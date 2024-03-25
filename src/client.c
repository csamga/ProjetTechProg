#include "client.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

struct client {
    char name[50];
    char phone[12];
    char email[50];
    char address[50];
    FILE *purchase_db;
};

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

    fwrite(&client, sizeof client, 1, client_db);

    fclose(client_db);
}

void print_client(const char *name) {
    FILE *client_db;
    struct client client;
    bool found;

    client_db = fopen("client_db.dat", "rb");

    if (client_db == NULL) {
        fprintf(stderr, "error: failed to open %s\n", "client_db.dat");
        return;
    }

    found = false;

    while (!feof(client_db) && !found) {
        fread(&client, sizeof client, 1, client_db);
        found = (strcmp(client.name, name) == 0);
    }

    if (found) {
        puts("Client informations:");
        printf("Client: %s\n", client.name);
        printf("Phone: %s\n", client.phone);
        printf("Email: %s\n", client.email);
        printf("Address: %s\n", client.address);

    } else {
        printf("Client \"%s\" does not exist", name);
    }

    fclose(client_db);
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
