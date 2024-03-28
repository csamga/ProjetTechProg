#ifndef FOURNISSEUR_H_INCLUDED
#define FOURNISSEUR_H_INCLUDED

#include <stdbool.h>
#include <stdio.h>

struct fournisseur {
    short id;
    char name[50];
    char phone[12];
    char email[50];
    char address[50];
};

void fournissseur_delete(void);
    
void fournisseur_register(void);

void fournisseur_search_by_name(
    const char *name,
    struct fournisseur *fournisseur,
    bool *exists
);

void fournisseur_search_by_id(
    const unsigned short id,
    struct fournisseur *fournisseur,
    bool *exists
);

void fournisseur_inspect(void);

#endif
