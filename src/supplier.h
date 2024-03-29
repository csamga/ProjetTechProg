#ifndef SUPPLIER_H_INCLUDED
#define SUPPLIER_H_INCLUDED

#include "address.h"

#include <stdbool.h>

struct supplier {
    unsigned short id;
    char last_name[32];
    char first_name[32];
    char phone[11];
    char email[64];
    struct address address;
};

void supplier_register(void);
void supplier_modify(void);
void supplier_inspect(void);
void supplier_delete(void);

void supplier_search_by_name(
    const char *name,
    struct supplier *supplier,
    bool *exists
);

void supplier_search_by_id(
    const unsigned short id,
    struct supplier *supplier,
    bool *exists
);

#endif
