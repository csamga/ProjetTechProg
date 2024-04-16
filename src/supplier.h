#ifndef SUPPLIER_H_INCLUDED
#define SUPPLIER_H_INCLUDED

#include "address.h"
#include "product.h"

#include <stdio.h>
#include <stdbool.h>

#define MAX_PRODUCTS_PER_COMMAND 20

struct supplier {
    unsigned short id;
    char last_name[32];
    char first_name[32];
    char phone[11];
    char email[64];
    struct address address;
    unsigned short n_order;
    char order_db_file_name[64];
};

struct order {
    unsigned short supplier_id;
    unsigned short order_id;
    unsigned short product_id[MAX_PRODUCTS_PER_COMMAND];
    unsigned short quantity[MAX_PRODUCTS_PER_COMMAND];
    unsigned short n_products;
    float total_euro;
};

void supplier_register(void);
void supplier_modify(void);
void supplier_inspect(void);
void supplier_delete(void);

void supplier_place_order(void);
void supplier_inspect_order(void);
void supplier_register_delivery(void);

void supplier_search_by_name(
    FILE *supplier_db,
    const char *name,
    struct supplier *supplier,
    long *pos
);

void supplier_search_by_id(
    const unsigned short id,
    struct supplier *supplier,
    bool *exists
);

#endif
