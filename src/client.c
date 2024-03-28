#include "client.h"
#include "input.h"
#include "terminal.h"

#include <string.h>

static unsigned short n_clients = 0;

static void client_read(struct client *client);

void client_register(void) {
    FILE *client_db;
    struct client client;
    
    client_db = fopen("client_db.dat", "ab");

    if (client_db == NULL) {
        fprintf(stderr, "error: failed to open %s\n", "client_db.dat");
        return;
    }

    client_read(&client);
    client.id = n_clients++;

    fwrite(&client, sizeof client, 1, client_db);

    fclose(client_db);

    puts("Client créé avec succès");
    getchar();
}

void client_modify(void) {
    FILE *client_db;
    struct client client;
    bool exists;
    char name[50];
    
    client_db = fopen("client_db.dat", "rb+");

    if (client_db == NULL) {
        fprintf(stderr, "error: failed to open %s\n", "client_db.dat");
        return;
    }

    fputs("Nom : ", stdout);
    fgets(name, sizeof name, stdin);
    name[strcspn(name, "\n")] = '\0';

    client_search_by_name(name, &client, &exists);

    if (!exists) {
        return;
    }

    client_read(&client);

    fwrite(&client, sizeof client, 1, client_db);

    fclose(client_db);
}

void client_delete(void) {
    FILE *old_client_db, *new_client_db;
    char name[50];
    struct client tmp;
    bool exists;

    fputs("Nom : ", stdout);
    fgets(name, sizeof name, stdin);
    name[strcspn(name, "\n")] = '\0';

    client_search_by_name(name, NULL, &exists);

    if (!exists) {
        return;
    }

    rename("client_db.dat", "tmp_client_db.dat");

    old_client_db = fopen("tmp_client_db.dat", "rb");

    if (old_client_db == NULL) {
        fprintf(stderr, "error: failed to open %s\n", "client_db.dat");
        return;
    }

    new_client_db = fopen("client_db.dat", "a");

    if (new_client_db == NULL) {
        fprintf(stderr, "error: failed to open %s\n", "new_client_db.dat");
        return;
    }

    while (!feof(old_client_db)) {
        fread(&tmp, sizeof tmp, 1, old_client_db);

        if (strcmp(name, tmp.first_name) != 0) {
            fwrite(&tmp, sizeof tmp, 1, new_client_db);
        }
    }

    fclose(old_client_db);
    fclose(new_client_db);

    remove("tmp_client_db.dat");
}

void client_search_by_name(
    const char *name,
    struct client *client,
    bool *exists)
{
    FILE *client_db;
    struct client tmp;

    client_db = fopen("client_db.dat", "rb");

    if (client_db == NULL) {
        fprintf(stderr, "error: failed to open %s\n", "client_db.dat");
        return;
    }

    *exists = false;

    while (!feof(client_db) && !*exists) {
        fread(&tmp, sizeof tmp, 1, client_db);
        *exists = (strcmp(tmp.last_name, name) == 0);
    }

    if (exists && *exists && client) {
        *client = tmp;
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

    client_db = fopen("client_db.dat", "rb");

    if (client_db == NULL) {
        fprintf(stderr, "error: failed to open %s\n", "client_db.dat");
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
        printf("Le client n°%hd n'existe pas", id);
        getchar();
    }

    fclose(client_db);
}

void client_inspect(void) {
    struct client client;
    char name[50];
    bool exists;

    fputs("Nom client : ", stdout);
    fgets(name, sizeof name, stdin);
    name[strcspn(name, "\n")] = '\0';

    client_search_by_name(name, &client, &exists);

    clear_screen();
    set_cursor_home();

    if (exists) {
        puts("Informations client");
        printf("Identifiant: %hd\n", client.id);
        printf("Nom: %s\n", client.last_name);
        printf("Prénom: %s\n", client.first_name);
        printf("Tel: %s\n", client.phone);
        printf("Email: %s\n", client.email);
        address_inspect(&client.address);

        getchar();
    }
}

static void client_read(struct client *client) {
    char *tmp;
    size_t len;
    bool valid;

    puts("Saisir informations client :");

    /* Saisie du nom */
    fputs("Nom : ", stdout);

    do {
        input_read_stdin(&tmp, &len);
        valid = input_validate_name(tmp, len);
    } while (!valid);

    strncpy(client->last_name, tmp, sizeof client->last_name);

    /* Saisie du prénom */
    fputs("Prénom : ", stdout);

    do {
        input_read_stdin(&tmp, &len);
        valid = input_validate_name(tmp, len);
    } while (!valid);

    strncpy(client->first_name, tmp, sizeof client->first_name);

    /* Saisie du numéro de téléphone */
    fputs("Tel : ", stdout);

    do {
        input_read_stdin(&tmp, &len);
        valid = input_validate_phone(tmp, len);
    } while (!valid);

    strncpy(client->phone, tmp, sizeof client->phone);

    /* Saisie de l'adresse email */
    fputs("Email : ", stdout);

    do {
        input_read_stdin(&tmp, &len);
        valid = input_validate_email(tmp, len);
    } while (false);

    strncpy(client->email, tmp, sizeof client->email);

    /* Saisie de l'adresse */
    address_read(&client->address);

    free(tmp);
}
