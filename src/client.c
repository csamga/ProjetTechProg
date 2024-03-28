#include "client.h"

#include <string.h>
#include <ctype.h>

static short n_clients = 0;

static struct client read_client(void);

void client_register(void) {
    FILE *client_db;
    struct client client;
    
    client_db = fopen("client_db.dat", "ab");

    if (client_db == NULL) {
        fprintf(stderr, "error: failed to open %s\n", "client_db.dat");
        return;
    }

    client = read_client();
    client.id = n_clients++;

    fwrite(&client, sizeof client, 1, client_db);

    fclose(client_db);
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

    client = read_client();

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

    old_client_db = fopen("client_db.dat", "rb");

    if (old_client_db == NULL) {
        fprintf(stderr, "error: failed to open %s\n", "client_db.dat");
        return;
    }

    new_client_db = fopen("tmp_client_db.dat", "a");

    if (new_client_db == NULL) {
        fprintf(stderr, "error: failed to open %s\n", "new_client_db.dat");
        return;
    }

    while (!feof(old_client_db)) {
        fread(&tmp, sizeof tmp, 1, old_client_db);

        if (strcmp(name, tmp.name) != 0) {
            fwrite(&tmp, sizeof tmp, 1, new_client_db);
        }
    }

    fclose(old_client_db);
    fclose(new_client_db);

    remove("client_db.dat");
    rename("tmp_client_db.dat", "client_db.dat");
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
        *exists = (strcmp(tmp.name, name) == 0);
    }

    if (exists && *exists && client) {
        *client = tmp;
    } else {
        client = NULL;
        printf("Le client \"%s\" n'existe pas", name);
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
        *client = tmp;
    } else {
        client = NULL;
        printf("Le client n°%hd n'existe pas", id);
    }

    fclose(client_db);
}

void client_print(void) {
    struct client client;
    char name[50];
    bool exists;

    fputs("Nom client : ", stdout);
    fgets(name, sizeof name, stdin);
    name[strcspn(name, "\n")] = '\0';

    client_search_by_name(name, &client, &exists);

    if (exists) {
        puts("Client informations:");
        printf("Client: %hd\n", client.id);
        printf("Client: %s\n", client.name);
        printf("Phone: %s\n", client.phone);
        printf("Email: %s\n", client.email);
        printf("Address: %s\n", client.address);
    }
}

static struct client read_client(void) {
#define CHUNK 256
    struct client client;
    char tmp[CHUNK];
    bool valid;
    size_t len, i;
    char c;
    bool has_at;
    char local[100], domain[100];

    puts("Saisir informations client:");

    /* Saisie nom */
    do {
        fputs("Nom: ", stdout);
        fgets(tmp, sizeof tmp, stdin);
        tmp[strcspn(tmp, "\n")] = '\0';

        len = strlen(tmp);
        valid = true;

        for (i = 0; i < len && valid; i++) {
            c = tmp[i];
            valid = (isalpha(c) || isspace(c));
        }

        if (!valid) {
            puts("Le nom ne peut comporter que des lettres et des espaces");
        }
    } while (!valid);

    strncpy(client.name, tmp, sizeof client.name);

    /* Saisie tel */
    do {
        fputs("Tel: ", stdout);
        fgets(tmp, sizeof tmp, stdin);
        tmp[strcspn(tmp, "\n")] = '\0';

        len = strlen(tmp);
        valid = (len == 10);

        for (i = 0; i < len && valid; i++) {
            c = tmp[i];
            valid = isdigit(c);
        }

        if (!valid) {
            puts("Le tel doit comporter 10 chiffres");
        }
    } while (!valid);

    strncpy(client.phone, tmp, sizeof client.phone);

    do {
        fputs("Email: ", stdout);
        fgets(tmp, sizeof tmp, stdin);
        tmp[strcspn(tmp, "\n")] = '\0';

        sscanf(tmp, "%s@%s", NULL, NULL);
    } while (false);

    fputs("Address: ", stdout);
    fgets(client.address, sizeof client.address, stdin);
    client.address[strcspn(client.address, "\n")] = '\0';

    puts("");

    return client;
}
