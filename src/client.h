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
    /* char purchase_db_file_name[32]; */
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

void client_inspect(void);

#endif
