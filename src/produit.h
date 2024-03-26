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
};

void register_product(void);

void search_product_by_name(
    const char *name,
    struct product *product,
    bool *exists
);

void search_product_by_id(
    unsigned short id,
    struct product *product,
    bool *exists
);

void print_product(struct product *product);

#endif
