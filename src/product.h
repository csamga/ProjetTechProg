#ifndef PRODUCT_H_INCLUDED
#define PRODUCT_H_INCLUDED

#include <stdbool.h>
#include <stdio.h>

struct product {
    unsigned short id;
    char name[32];
    char brand[32];
    char origin[32];
    float price_euro;
    float mass_kg_vol_l;
    unsigned short quantity;
};

void product_register(void);
void product_modify(void);
void product_inspect(void);
void product_delete(void);
void product_inspect_inventory(void);

void product_search_by_name(FILE *product_db, const char *name, struct product *product, long *pos);
void product_search_by_id(FILE *product_db, unsigned short id, struct product *product, long *pos);

bool product_db_exists(FILE *product_db);

#endif
