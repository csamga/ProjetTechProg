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
static void client_print(struct client *client);

void client_register(void) {
    FILE *client_db;
    struct client client;
    
    client_db = fopen(CLIENT_DB, "ab");

    if (client_db == NULL) {
        log_error("erreur: impossible d'ouvrir " CLIENT_DB);
        return;
    }

    client.id = market_get_n_clients();
    client_read(&client);
    client_create_db(&client);

    fwrite(&client, sizeof client, 1, client_db);
    fclose(client_db);

    market_client_added();

    log_success("Client créé avec succès");
}

void client_modify(void) {
    FILE *client_db;
    struct client client;
    char name[32];
    bool finished;
    int choice;
    long pos, end;

    log_title("Modification client\n");
    
    client_db = fopen(CLIENT_DB, "rb+");
    end = 0l;

    if (client_db) {
        fseek(client_db, 0l, SEEK_END);
        end = ftell(client_db);
    }

    if (end == 0l) {
        if (client_db != NULL) {
            fclose(client_db);
        }

        log_info("La base de données client est vide");
        return;
    }

    input_read_alpha("Nom : ", name, sizeof name);
    client_search_by_name(client_db, name, &client, &pos);

    if (pos == -1l) {
        return;
    }

    do {
        new_page();

        log_title("Champ à modifier\n");
        list_print(field_str, 5, 1);

        choice = menu_acquire_input();
        finished = false;

        switch (choice) {
        case 0:
            printf("\nAncien nom : %s\n", client.last_name);
            input_read_alpha("Nouveau nom : ", client.last_name, sizeof client.last_name);
            break;
        case 1:
            printf("\nAncien prénom : %s\n", client.first_name);
            input_read_alpha("Nouveau prénom : ", client.first_name, sizeof client.first_name);
            break;
        case 2:
            printf("\nAncien tel : %s\n", client.phone);
            input_read_num(
                "Nouveau tel : ",
                "Le tel doit contenir exactement 10 chiffres",
                client.phone,
                sizeof client.phone,
                10
            );
            break;
        case 3:
            printf("\nAncien email : %s\n", client.email);
                input_read_email("Nouvel email : ", "Adresse email invalide", client.email, sizeof client.email);
            break;
        case 4:
            finished = true;
        }
    } while (!finished);

    fseek(client_db, pos, SEEK_SET);
    fwrite(&client, sizeof client, 1, client_db);
    fclose(client_db);

    log_success("Client modifié avec succès");
}

void client_inspect(void) {
    FILE *client_db;
    struct client client;
    char name[32];
    long pos, end;

    log_title("Informations client\n");
    
    client_db = fopen(CLIENT_DB, "rb+");
    end = 0l;

    if (client_db) {
        fseek(client_db, 0l, SEEK_END);
        end = ftell(client_db);
    }

    if (end == 0l) {
        if (client_db != NULL) {
            fclose(client_db);
        }

        log_info("La base de données client est vide");
        return;
    }

    input_read_alpha("Nom : ", name, sizeof name);
    client_search_by_name(client_db, name, &client, &pos);

    if (pos != -1l) {
        move_cursor_up(1);
        client_print(&client);
        getchar();
    }
}

void client_delete(void) {
    FILE *old_client_db, *new_client_db;
    struct client client;
    char name[32];
    bool valid;
    long pos, end;

    log_title("Suppression client\n");

    old_client_db = fopen(CLIENT_DB, "rb+");
    end = 0l;

    if (old_client_db) {
        fseek(old_client_db, 0l, SEEK_END);
        end = ftell(old_client_db);
    }

    if (end == 0l) {
        if (old_client_db != NULL) {
            fclose(old_client_db);
        }

        log_info("La base de données client est vide");
        return;
    }

    input_read_alpha("Nom : ", name, sizeof name);
    client_search_by_name(old_client_db, name, &client, &pos);

    if (pos == -1l) {
        return;
    }

    valid = input_confirm_delete("Voulez vous vraiment supprimer le client ?");

    if (!valid) {
        return;
    }

    rename(CLIENT_DB, "db/old_client_db.dat");

    old_client_db = fopen("db/old_client_db.dat", "rb");

    if (old_client_db == NULL) {
        log_error("erreur: le fichier db/old_client_db.dat n'existe pas");
        return;
    }

    new_client_db = fopen(CLIENT_DB, "a");

    if (new_client_db == NULL) {
        log_error("erreur: impossible d'ouvrir " CLIENT_DB);
        return;
    }

    while (!feof(old_client_db)) {
        if (fread(&client, sizeof client, 1, old_client_db)) {
            if (strcmp(name, client.last_name) != 0) {
                fwrite(&client, sizeof client, 1, new_client_db);
            }
        }
    }

    fclose(old_client_db);
    fclose(new_client_db);

    remove("db/old_client_db.dat");
    remove(client.purchase_db_file_name);

    log_success("Client supprimé avec succès");
}

void client_print_history(void) {
    FILE *client_db, *per_client_db;
    struct client client;
    char name[32], line[80];
    long pos, end;

    new_page();

    log_title("Historique achat\n");

    client_db = fopen(CLIENT_DB, "rb+");
    end = 0l;

    if (client_db) {
        fseek(client_db, 0l, SEEK_END);
        end = ftell(client_db);
    }

    if (end == 0l) {
        if (client_db != NULL) {
            fclose(client_db);
        }

        log_info("La base de données client est vide");
        return;
    }

    input_read_alpha("Nom : ", name, sizeof name);
    client_search_by_name(client_db, name, &client, &pos);
    fclose(client_db);

    if (pos == -1l) {
        return;
    }

    per_client_db = fopen(client.purchase_db_file_name, "rb");

    if (per_client_db == NULL) {
        log_error("erreur: impossible d'ouvrir %s", client.purchase_db_file_name);
        return;
    }

    fseek(per_client_db, 0l, SEEK_END);
    end = ftell(per_client_db);

    if (end == 0l) {
        log_info("L'historique d'achat du client %s %s est vide",
            client.last_name,
            client.first_name
        );
        return;
    }

    rewind(per_client_db);
    puts("");

    while (!feof(per_client_db)) {
        if (fgets(line, sizeof line, per_client_db)) {
            fputs(line, stdout);
        }
    }

    fclose(per_client_db);

    getchar();
}

void client_search_by_name(
    FILE *client_db,
    const char *name,
    struct client *client,
    long *pos)
{
    struct client tmp;
    bool exists;

    rewind(client_db);

    exists = false;

    while (!feof(client_db) && !exists) {
        fread(&tmp, sizeof tmp, 1, client_db);
        exists = (strcmp(tmp.last_name, name) == 0);
    }

    if (exists) {
        if (client) {
            *client = tmp;
        }
        if (pos) {
            *pos = ftell(client_db) - sizeof tmp;
        }
    } else {
        if (pos) {
            *pos = -1l;
        }

        log_info("\nLe client \"%s\" n'existe pas", name);
    }
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
        log_error("erreur: le fichier " CLIENT_DB " n'existe pas");
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
        log_info("Le client n°%hu n'existe pas", id);
    }

    fclose(client_db);
}

static void client_read(struct client *client) {
    log_title("Saisie informations client\n");

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
    input_read_email("Email : ", "Adresse email invalide", client->email, sizeof client->email);

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
        log_error("erreur: le fichier %s existe déjà", tmp);
        return;
    }

    fclose(db);
}

static void client_print(struct client *client) {
    printf("Identifiant : %hu\n", client->id);
    printf("Nom : %s\n", client->last_name);
    printf("Prénom : %s\n", client->first_name);
    printf("Tel : %s\n", client->phone);
    printf("Email : %s\n", client->email);
    address_inspect(&client->address);
}
