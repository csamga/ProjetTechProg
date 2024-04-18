#ifndef CLIENT_H_INCLUDED
#define CLIENT_H_INCLUDED

#include "address.h"

#include <stdbool.h>
#include <stdio.h>

struct client {
    unsigned short id;
    char last_name[32];
    char first_name[32];
    char phone[12];
    char email[64];
    struct address address;
    char purchase_db_file_name[64];
};

void client_register(void);
void client_modify(void);
void client_inspect(void);
void client_delete(void);
void client_print_history(void);

void client_search_by_name(
    FILE *client_db,
    const char *name,
    struct client *client,
    long *pos
);
void client_search_by_id(
    const unsigned short id,
    struct client *client,
    bool *exists
);

bool client_db_exists(FILE *client_db);

#endif
