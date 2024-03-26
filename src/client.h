#ifndef CLIENT_H_INCLUDED
#define CLIENT_H_INCLUDED

#include <stdbool.h>
#include <stdio.h>

struct client {
    short id;
    char name[50];
    char phone[12];
    char email[50];
    char address[50];
    FILE *purchase_db;
};

void register_client(void);

void search_client_by_name(
    const char *name,
    struct client *client,
    bool *exists
);

void search_client_by_id(
    unsigned short id,
    struct client *client,
    bool *exists
);

void print_client(struct client *client);

#endif
