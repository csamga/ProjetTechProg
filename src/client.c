#include "client.h"
#include "input.h"
#include "terminal.h"
#include "market.h"
#include "utils.h"

#include <string.h>
#include <stdio.h>

static char *field_str[] = {
    "Nom",
    "Prenom",
    "Tel",
    "Email",
    "Retour"
};

static void client_read(struct client *client);
static void client_create_db(struct client *client);

void client_register(void) {
    FILE *client_db;
    struct client client;
    
    client_db = fopen(CLIENT_DB, "ab");

    if (client_db == NULL) {
        fputs("erreur: impossible d'ouvrir " CLIENT_DB "\n", stderr);
        return;
    }

    client.id = market_get_n_clients();
    client_create_db(&client);
    client_read(&client);

    fwrite(&client, sizeof client, 1, client_db);
    fclose(client_db);

    market_client_added();

    new_page();
    puts("Client créé avec succès");
    getchar();
}

void client_modify(void) {
    FILE *client_db;
    struct client client;
    char name[32];
    bool valid, finished;
    int choice;
    
    client_db = fopen(CLIENT_DB, "rb+");

    if (client_db == NULL) {
        fputs("erreur: impossible d'ouvrir " CLIENT_DB "\n", stderr);
        return;
    }

    /* Saisie du nom */
    input_read_alpha("Nom : ", name, sizeof name);
    client_search_by_name(name, &client, &valid);

    if (!valid) {
        return;
    }

    do {
        new_page();
        puts("Champ à modifier");
        list_print(field_str, 5, 1);

        choice = acquire_input();
        finished = false;
        new_page();

        switch (choice) {
        case 0:
            printf("Ancien nom : %s\n", client.last_name);
            input_read_alpha("Nouveau nom : ", client.last_name, sizeof client.last_name);
            break;
        case 1:
            printf("Ancien prénom : %s\n", client.first_name);
            input_read_alpha("Nouveau prénom : ", client.first_name, sizeof client.first_name);
            break;
        case 2:
            printf("Ancien tel : %s\n", client.phone);
            input_read_num(
                "Nouveau tel : ",
                "Le tel doit contenir exactement 10 chiffres",
                client.phone,
                sizeof client.phone,
                10
            );
            break;
        case 3:
            printf("Ancien email : %s\n", client.email);
            input_read_email(client.email, sizeof client.email);
            break;
        case 4:
            finished = true;
        }
    } while (!finished);

    fwrite(&client, sizeof client, 1, client_db);
    fclose(client_db);

    new_page();
    puts("Client modifié avec succès");
    getchar();
}

void client_inspect(void) {
    struct client client;
    char name[32];
    bool valid;

    /* Saisie du nom */
    input_read_alpha("Nom : ", name, sizeof name);
    client_search_by_name(name, &client, &valid);

    if (valid) {
        new_page();

        puts("Informations client");
        printf("Identifiant : %hu\n", client.id);
        printf("Nom : %s\n", client.last_name);
        printf("Prénom : %s\n", client.first_name);
        printf("Tel : %s\n", client.phone);
        printf("Email : %s\n", client.email);
        address_inspect(&client.address);

        getchar();
    }
}

void client_delete(void) {
    FILE *old_client_db, *new_client_db;
    struct client client;
    char name[32];
    bool valid;

    /* Saisie du nom */
    input_read_alpha("Nom : ", name, sizeof name);
    client_search_by_name(name, &client, &valid);

    if (!valid) {
        return;
    }

    valid = input_confirm_delete("Voulez vous vraiment supprimer le client ?");

    if (!valid) {
        return;
    }

    rename(CLIENT_DB, "db/old_client_db.dat");

    old_client_db = fopen("db/old_client_db.dat", "rb");

    if (old_client_db == NULL) {
        fputs("erreur: impossible d'ouvrir db/old_client_db.dat\n", stderr);
        return;
    }

    new_client_db = fopen(CLIENT_DB, "a");

    if (new_client_db == NULL) {
        fputs("erreur: impossible d'ouvrir " CLIENT_DB "\n", stderr);
        return;
    }

    while (!feof(old_client_db)) {
        fread(&client, sizeof client, 1, old_client_db);

        if (strcmp(name, client.last_name) != 0) {
            fwrite(&client, sizeof client, 1, new_client_db);
        }
    }

    fclose(old_client_db);
    fclose(new_client_db);

    remove("db/old_client_db.dat");
    remove(client.purchase_db_file_name);

    new_page();
    puts("Client supprimé avec succès");
    getchar();
}

void client_print_history(void) {
    FILE *client_db;
    struct client client;
    char name[32], line[80];
    bool exists;

    /* Saisie du nom */
    new_page();
    input_read_alpha("Nom : ", name, sizeof name);
    client_search_by_name(name, &client, &exists);

    if (!exists) {
        return;
    }

    client_db = fopen(client.purchase_db_file_name, "rb");

    if (client_db == NULL) {
        fprintf(stderr, "erreur: impossible d'ouvrir %s", client.purchase_db_file_name);
        getchar();
        return;
    }

    new_page();

    while (!feof(client_db)) {
        if (fgets(line, sizeof line, client_db)) {
            fputs(line, stdout);
        }
    }

    fclose(client_db);

    getchar();
}

void client_search_by_name(
    const char *name,
    struct client *client,
    bool *exists)
{
    FILE *client_db;
    struct client tmp;

    client_db = fopen(CLIENT_DB, "rb");

    if (client_db == NULL) {
        fputs("erreur: impossible d'ouvrir " CLIENT_DB "\n", stderr);
        return;
    }

    *exists = false;

    while (!feof(client_db) && (!*exists)) {
        fread(&tmp, sizeof tmp, 1, client_db);
        *exists = (strcmp(tmp.last_name, name) == 0);
    }

    if (exists && *exists) {
        if (client) {
            *client = tmp;
        }
    } else {
        client = NULL;
        printf("Le client \"%s\" n'existe pas", name);
        getchar();
    }

    fclose(client_db);
}

void client_search_by_id(
    const unsigned short id,
    struct client *client,
    bool *exists)
{
    FILE *client_db;
    struct client tmp;

    client_db = fopen(CLIENT_DB, "rb");

    if (client_db == NULL) {
        fputs("erreur: impossible d'ouvrir " CLIENT_DB "\n", stderr);
        return;
    }

    *exists = false;

    while (!feof(client_db) && !*exists) {
        fread(&tmp, sizeof tmp, 1, client_db);
        *exists = (tmp.id == id);
    }

    if (exists && *exists) {
        if (client) {
            *client = tmp;
        }
    } else {
        client = NULL;
        printf("Le client n°%hu n'existe pas", id);
        getchar();
    }

    fclose(client_db);
}

static void client_read(struct client *client) {
    puts("Saisie informations client");

    /* Saisie du nom */
    input_read_alpha("Nom : ", client->last_name, sizeof client->last_name);

    /* Saisie du prénom */
    input_read_alpha("Prénom : ", client->first_name, sizeof client->first_name);

    /* Saisie du numéro de téléphone */
    input_read_num(
        "Tel : ",
        "Le tel doit contenir exactement 10 chiffres",
        client->phone,
        sizeof client->phone,
        10
    );

    /* Saisie de l'adresse email */
    input_read_email(client->email, sizeof client->email);

    /* Saisie de l'adresse */
    address_read(&client->address);
}

static void client_create_db(struct client *client) {
    FILE *db;
    char tmp[64], num_str[8];

    strncpy(tmp, PER_CLIENT_DB_PREFIX, sizeof tmp);
    sprintf(num_str, "%d", client->id);
    strncat(tmp, num_str, sizeof tmp - strlen(num_str));
    strncat(tmp, PER_CLIENT_DB_SUFFIX, sizeof tmp - strlen(PER_CLIENT_DB_SUFFIX));
    strncpy(client->purchase_db_file_name, tmp, sizeof client->purchase_db_file_name);

    db = fopen(client->purchase_db_file_name, "wx");

    if (db == NULL) {
        fprintf(stderr, "erreur: impossible de créer le fichier %s", tmp);
        return;
    }

    fclose(db);
}
