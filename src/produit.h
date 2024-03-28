#ifndef PRODUIT_H_INCLUDED
#define PRODUIT_H_INCLUDED

#include <stdbool.h>

struct product {
    short id;
    char name[50];
    char brand[50];
    float price_euro;
    char origin[50];
    bool liquid;
    union juan {
        float mass_kg;
        float volume_l;
    } juan;
    unsigned short stock;
};

void product_register(void);
void product_modify(void);
void product_delete(void);

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

void product_inspect(void);

#endif
