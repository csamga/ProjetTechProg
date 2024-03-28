#ifndef CLIENT_H_INCLUDED
#define CLIENT_H_INCLUDED

#include <stdbool.h>
#include <stdio.h>

struct client {
    short id;
    char name[50];
    char phone[12];
    char email[100];
    char address[50];
    FILE *purchase_db;
};

void client_register(void);
void client_modify(void);
void client_delete(void);

void client_search_by_name(
    const char *name,
    struct client *client,
    bool *exists
);

void client_search_by_id(
    const unsigned short id,
    struct client *client,
    bool *exists
);

void client_print(void);

#endif
