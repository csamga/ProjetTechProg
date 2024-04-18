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
    
    /* ouvrir la db client */
    client_db = fopen(CLIENT_DB, "ab");

    if (client_db == NULL) {
        log_error("erreur: impossible d'ouvrir " CLIENT_DB);
        return;
    }

    /* créer le client */
    client.id = market_get_n_clients();
    client_read(&client);
    client_create_db(&client);

    /* écrire le client dans la db */
    fwrite(&client, sizeof client, 1, client_db);
    fclose(client_db);

    /* notifier de l'enregistrement */
    market_client_added();

    log_success("Client créé avec succès");
}

void client_modify(void) {
    FILE *client_db;
    struct client client;
    char name[32];
    bool finished, modified;
    int choice;
    long pos;

    log_title("Modification client");
    
    /* ouvrir la db client */
    client_db = fopen(CLIENT_DB, "rb+");

    if (!client_db_exists(client_db)) {
        return;
    }

    /* chercher le client dans la db */
    input_read_alpha("Nom : ", name, sizeof name);
    client_search_by_name(client_db, name, &client, &pos);

    if (pos == -1l) {
        fclose(client_db);
        return;
    }

    modified = false;

    do {
        new_page();

        log_title("Champ à modifier");
        list_print(field_str, 5, 1);

        /* choisir le champ à modifier */
        choice = menu_acquire_input();
        finished = false;

        /* modifier le champ */
        switch (choice) {
        case 0:
            printf("\nAncien nom  : %s\n", client.last_name);
            input_read_alpha(
                "Nouveau nom : ",
                client.last_name,
                sizeof client.last_name
            );
            modified = true;
            break;
        case 1:
            printf("\nAncien prénom  : %s\n", client.first_name);
            input_read_alpha(
                "Nouveau prénom : ",
                client.first_name,
                sizeof client.first_name
            );
            modified = true;
            break;
        case 2:
            printf("\nAncien tel  : %s\n", client.phone);
            input_read_num(
                "Nouveau tel : ",
                "Le tel doit contenir exactement 10 chiffres",
                client.phone,
                sizeof client.phone,
                10
            );
            modified = true;
            break;
        case 3:
            printf("\nAncien email : %s\n", client.email);
            input_read_email(
                "Nouvel email : ",
                "Adresse email invalide",
                client.email,
                sizeof client.email
            );
            modified = true;
            break;
        case 4:
            finished = true;
        }
    } while (!finished);

    if (modified) {
        /* remplacer l'enregistrement */
        fseek(client_db, pos, SEEK_SET);
        fwrite(&client, sizeof client, 1, client_db);
        log_success("Client modifié avec succès");
    } else {
        log_info(true, "Le client n'a pas été modifié");
    }

    fclose(client_db);
}

void client_inspect(void) {
    FILE *client_db;
    struct client client;
    char name[32];
    long pos;

    log_title("Informations client");

    /* ouvrir la db client */
    client_db = fopen(CLIENT_DB, "rb");

    if (!client_db_exists(client_db)) {
        return;
    }

    /* chercher le client dans la base de données */
    input_read_alpha("Nom : ", name, sizeof name);
    client_search_by_name(client_db, name, &client, &pos);

    if (pos != -1l) {
        /* afficher informations client */
        move_cursor_up(1);
        client_print(&client);
        getchar();
    }

    fclose(client_db);
}

void client_delete(void) {
    FILE *client_db, *new_client_db;
    struct client client;
    char name[32];
    bool delete;
    long pos;

    log_title("Suppression client");

    /* ouvrir la db client */
    client_db = fopen(CLIENT_DB, "rb");

    if (!client_db_exists(client_db)) {
        return;
    }

    /* chercher le client dans la db */
    input_read_alpha("Nom : ", name, sizeof name);
    client_search_by_name(client_db, name, &client, &pos);

    if (pos == -1l) {
        fclose(client_db);
        return;
    }

    delete = input_confirm_delete("Voulez vous vraiment supprimer le client ?");

    if (!delete) {
        fclose(client_db);
        return;
    }

    /* ouvrir la nouvelle db client */
    new_client_db = fopen("db/new_client_db.dat", "ab");

    if (new_client_db == NULL) {
        log_error("erreur: impossible d'ouvrir db/new_client_db.dat");
        fclose(client_db);
        return;
    }

    rewind(client_db);

    /* copier le contenu de la db client dans la nouvelle db
     * excepté le client choisi */
    while (!feof(client_db)) {
        if (fread(&client, sizeof client, 1, client_db)) {
            if (strcmp(name, client.last_name) != 0) {
                fwrite(&client, sizeof client, 1, new_client_db);
            }
        }
    }

    fclose(client_db);
    remove(CLIENT_DB);

    fclose(new_client_db);
    rename("db/new_client_db.dat", CLIENT_DB);

    remove(client.purchase_db_file_name);

    log_success("Client supprimé avec succès");
}

void client_print_history(void) {
    FILE *client_db, *per_client_db;
    struct client client;
    char name[32], line[80];
    long pos;

    new_page();

    log_title("Historique d'achat");

    /* ouvrir la db client */
    client_db = fopen(CLIENT_DB, "rb");

    if (!client_db_exists(client_db)) {
        return;
    }

    /* chercher le client dans la db */
    input_read_alpha("Nom : ", name, sizeof name);
    client_search_by_name(client_db, name, &client, &pos);

    fclose(client_db);

    if (pos == -1l) {
        return;
    }

    /* ouvrir la db du client */
    per_client_db = fopen(client.purchase_db_file_name, "rb");

    if (per_client_db == NULL) {
        log_error("erreur: le fichier %s n'existe pas", client.purchase_db_file_name);
        return;
    }

    /* déterminer si la base de donnée du client est vide */
    fseek(per_client_db, 0l, SEEK_END);
    pos = ftell(per_client_db);

    if (pos == 0l) {
        log_info(
            true,
            "L'historique d'achat du client %s %s est vide",
            client.last_name,
            client.first_name
        );

        fclose(per_client_db);
        return;
    }

    rewind(per_client_db);
    puts("");

    /* afficher le contenu de la base de données du client */
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

    /* chercher le client dans la db client */
    while (!feof(client_db) && !exists) {
        if (fread(&tmp, sizeof tmp, 1, client_db)) {
            exists = (strcmp(tmp.last_name, name) == 0);
        }
    }

    if (exists) {
        /* retourner le client */
        if (client) {
            *client = tmp;
        }

        /* retourner la position de l'enregistrement */
        if (pos) {
            *pos = ftell(client_db) - sizeof tmp;
        }
    } else {
        if (pos) {
            *pos = -1l;
        }

        log_info(true, "Le client \"%s\" n'existe pas", name);
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
        log_info(true, "Le client n°%hu n'existe pas", id);
    }

    fclose(client_db);
}

bool client_db_exists(FILE *client_db) {
    long pos;

    pos = 0l;

    /* déterminer la position de fin de fichier */
    if (client_db) {
        fseek(client_db, 0l, SEEK_END);
        pos = ftell(client_db);
    }

    /* le fichier est vide si la position vaut 0 */
    if (pos == 0l) {
        if (client_db != NULL) {
            fclose(client_db);
        }

        log_info(true, "La base de données client est vide");
    }

    return (pos != 0l);
}

static void client_read(struct client *client) {
    log_title("Saisie informations client");

    /* saisie du nom */
    input_read_alpha("Nom : ", client->last_name, sizeof client->last_name);

    /* saisie du prénom */
    input_read_alpha("Prénom : ", client->first_name, sizeof client->first_name);

    /* saisie du numéro de téléphone */
    input_read_num(
        "Tel : ",
        "Le tel doit contenir exactement 10 chiffres",
        client->phone,
        sizeof client->phone,
        10
    );

    /* saisie de l'adresse email */
    input_read_email("Email : ", "Adresse email invalide", client->email, sizeof client->email);

    /* saisie de l'adresse */
    address_read(&client->address);
}

static void client_create_db(struct client *client) {
    FILE *db;
    char tmp[64], num_str[8];

    /* génerer le nom de la db du client en fonction de son ID */
    strncpy(tmp, PER_CLIENT_DB_PREFIX, sizeof tmp);
    sprintf(num_str, "%d", client->id);
    strncat(tmp, num_str, sizeof tmp - strlen(num_str));
    strncat(
        tmp,
        PER_CLIENT_DB_SUFFIX,
        sizeof tmp - strlen(PER_CLIENT_DB_SUFFIX)
    );
    strncpy(
        client->purchase_db_file_name,
        tmp,
        sizeof client->purchase_db_file_name
    );

    /* créer la db du client */
    db = fopen(client->purchase_db_file_name, "wx");

    if (db == NULL) {
        log_error("erreur: le fichier %s existe déjà", tmp);
        return;
    }

    fclose(db);
}

static void client_print(struct client *client) {
    printf(
        "Identifiant : %hu\n"
        "Nom         : %s\n"
        "Prénom      : %s\n"
        "Tel         : %s\n"
        "Email       : %s\n",
        client->id,
        client->last_name,
        client->first_name,
        client->phone,
        client->email
    );

    address_inspect(&client->address);
}
