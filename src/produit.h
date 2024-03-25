#ifndef PRODUIT_H_INCLUDED
#define PRODUIT_H_INCLUDED

#include <stdbool.h>

struct product {
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

struct product read_product(void);
void register_product(void);
void print_product(const char *name);

#endif
