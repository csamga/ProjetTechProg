#include "client.h"
#include "input.h"
#include "terminal.h"

#include <string.h>
#include <stdio.h>

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
    char name[32], *tmp;
    size_t len;
    bool valid;
    
    client_db = fopen("client_db.dat", "rb+");

    if (client_db == NULL) {
        fprintf(stderr, "error: failed to open %s\n", "client_db.dat");
        return;
    }

    /* Saisie du nom */
    fputs("Nom : ", stdout);

    do {
        input_read_stdin(&tmp, &len);
        valid = input_validate_name(tmp, len);
    } while (!valid);

    strncpy(name, tmp, sizeof name);

    client_search_by_name(name, &client, &valid);

    free(tmp);

    if (!valid) {
        return;
    }

    client_read(&client);

    fwrite(&client, sizeof client, 1, client_db);

    fclose(client_db);

    puts("Client modifié avec succès");
    getchar();
}

void client_inspect(void) {
    struct client client;
    char name[32], *tmp;
    size_t len;
    bool valid;

    /* Saisie du nom */
    fputs("Nom : ", stdout);

    do {
        input_read_stdin(&tmp, &len);
        valid = input_validate_name(tmp, len);
    } while (!valid);

    strncpy(name, tmp, sizeof name);

    client_search_by_name(name, &client, &valid);

    free(tmp);

    clear_screen();
    set_cursor_home();

    if (valid) {
        puts("Informations client");
        printf("Identifiant: %hu\n", client.id);
        printf("Nom: %s\n", client.last_name);
        printf("Prénom: %s\n", client.first_name);
        printf("Tel: %s\n", client.phone);
        printf("Email: %s\n", client.email);
        address_inspect(&client.address);

        getchar();
    }
}

void client_delete(void) {
    FILE *old_client_db, *new_client_db;
    struct client client;
    char name[32], *tmp;
    bool valid;
    size_t len;

    /* Saisie du nom */
    fputs("Nom : ", stdout);

    do {
        input_read_stdin(&tmp, &len);
        valid = input_validate_name(tmp, len);
    } while (!valid);

    strncpy(name, tmp, sizeof name);

    client_search_by_name(name, NULL, &valid);

    free(tmp);

    if (!valid) {
        return;
    }

    valid = input_confirm_delete("Voulez vous vraiment supprimer le client ?");

    if (!valid) {
        return;
    }

    rename("client_db.dat", "old_client_db.dat");

    old_client_db = fopen("old_client_db.dat", "rb");

    if (old_client_db == NULL) {
        fprintf(stderr, "error: failed to open %s\n", "old_client_db.dat");
        return;
    }

    new_client_db = fopen("client_db.dat", "a");

    if (new_client_db == NULL) {
        fprintf(stderr, "error: failed to open %s\n", "client_db.dat");
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

    remove("old_client_db.dat");

    puts("Client supprimé avec succès");
    getchar();
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
        printf("Le client n°%hu n'existe pas", id);
        getchar();
    }

    fclose(client_db);
}

static void client_read(struct client *client) {
    char *tmp;
    size_t len;
    bool valid;

    puts("Saisie informations client");

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
