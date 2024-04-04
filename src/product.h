#ifndef PRODUCT_H_INCLUDED
#define PRODUCT_H_INCLUDED

#include <stdbool.h>

struct product {
    unsigned short id;
    char name[32];
    char brand[32];
    char origin[32];
    float price_euro;
    float mass_kg_vol_l;
    /* unsigned short stock; */
};

void product_register(void);
void product_modify(void);
void product_inspect(void);
void product_delete(void);
void product_inspect_inventory(void);

void product_search_by_name(
    const char *name,
    struct product *product,
    bool *exists
);

void product_search_by_id(
    unsigned short id,
    struct product *product,
    bool *exists
);

#endif
