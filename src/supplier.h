#ifndef SUPPLIER_H_INCLUDED
#define SUPPLIER_H_INCLUDED

#include <stdbool.h>
#include <stdio.h>

struct supplier {
    short id;
    char name[50];
    char phone[12];
    char email[50];
    char address[50];
};

void supplier_register(void);

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

void supplier_inspect(void);

#endif
