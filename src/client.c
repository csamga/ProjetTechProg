#include "client.h"

#include <string.h>

static short n_clients = 0;

static struct client read_client(void);

void register_client(void) {
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

void search_client_by_name(
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

    if (exists) {
        *client = tmp;
    } else {
        client = NULL;
        printf("Le client \"%s\" n'existe pas", name);
    }

    fclose(client_db);
}

void search_client_by_id(
    unsigned short id,
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

    if (exists) {
        *client = tmp;
    } else {
        client = NULL;
        printf("Le client n°%hd n'existe pas", id);
    }

    fclose(client_db);
}

void print_client(struct client *client) {
    FILE *client_db;

    puts("Client informations:");
    printf("Client: %s\n", client->name);
    printf("Phone: %s\n", client->phone);
    printf("Email: %s\n", client->email);
    printf("Address: %s\n", client->address);

    fclose(client_db);
    puts("");
}

static struct client read_client(void) {
    struct client client;

    puts("Enter client informations:");

    fputs("Name: ", stdout);
    fgets(client.name, sizeof client.name, stdin);
    client.name[strcspn(client.name, "\n")] = '\0';

    fputs("Phone: ", stdout);
    fgets(client.phone, sizeof client.phone, stdin);
    client.phone[strcspn(client.phone, "\n")] = '\0';

    fputs("Email: ", stdout);
    fgets(client.email, sizeof client.email, stdin);
    client.email[strcspn(client.email, "\n")] = '\0';

    fputs("Address: ", stdout);
    fgets(client.address, sizeof client.address, stdin);
    client.address[strcspn(client.address, "\n")] = '\0';

    puts("");

    return client;
}
