#include "product.h"
#include "input.h"
#include "terminal.h"
#include "utils.h"
#include "market.h"

#include <stdio.h>
#include <string.h>

static char *field_str[] = {
    "Nom",
    "Marque",
    "Origine",
    "Prix",
    "Retour"
};

static void product_read(struct product *product);

void product_register(void) {
    FILE *product_db;
    struct product product;
    
    product_db = fopen("product_db.dat", "ab");

    if (product_db == NULL) {
        fprintf(stderr, "error: failed to open %s\n", "product_db.dat");
        return;
    }

    product_read(&product);
    product.id = market_get_n_products();
    market_product_added();

    fwrite(&product, sizeof product, 1, product_db);
    fclose(product_db);

    new_page();
    puts("Produit enregistré avec succès");
    getchar();
}

void product_modify(void) {
    FILE *product_db;
    struct product product;
    char name[32];
    bool valid, finished;
    int choice;
    
    product_db = fopen("product_db.dat", "rb+");

    if (product_db == NULL) {
        fprintf(stderr, "error: failed to open %s\n", "product_db.dat");
        return;
    }

    input_read_last_name(name, sizeof name);
    product_search_by_name(name, &product, &valid);

    if (!valid) {
        return;
    }

    do {
        new_page();
        puts("Champ à modifier");
        list_print(field_str, 5, 1);

        choice = acquire_input();
        finished = false;
        new_page();

        switch (choice) {
        case 0:
            printf("Ancien nom : %s\n", product.name);
            input_read_last_name(product.name, sizeof product.name);
            break;
        case 1:
            printf("Ancienne marque : %s\n", product.brand);
            input_read_last_name(product.brand, sizeof product.brand);
            break;
        case 2:
            printf("Ancienne origine : %s\n", product.origin);
            input_read_last_name(product.origin, sizeof product.origin);
            break;
        case 3:
            printf("Ancien prix : %f\n", product.price_euro);
            input_read_price(&product.price_euro);
            break;
        case 4:
            finished = true;
        }
    } while (!finished);


    fwrite(&product, sizeof product, 1, product_db);
    fclose(product_db);

    new_page();
    puts("Produit modifié avec succès");
    getchar();
}

void product_inspect(void) {
    struct product product;
    char name[32];
    bool valid;

    /* Saisie du nom */
    input_read_last_name(name, sizeof name);
    product_search_by_name(name, &product, &valid);

    if (valid) {
        new_page();

        puts("Informations produit");
        printf("Identifiant : %hu\n", product.id);
        printf("Nom : %s\n", product.name);
        printf("Marque : %s\n", product.brand);
        printf("Origine : %s\n", product.origin);
        printf("Prix : %.2f€\n", product.price_euro);
        printf("Masse/volume (kg/L) : %.3f\n", product.mass_kg_vol_l);

        getchar();
    }
}

void product_delete(void) {
    FILE *old_product_db, *new_product_db;
    struct product product;
    char name[32];
    bool valid;

    /* Saisie du nom */
    input_read_last_name(name, sizeof name);
    product_search_by_name(name, NULL, &valid);

    if (!valid) {
        return;
    }

    valid = input_confirm_delete("Voulez vous vraiment supprimer le produit ?");

    if (!valid) {
        return;
    }

    rename("product_db.dat", "old_product_db.dat");

    old_product_db = fopen("old_product_db.dat", "rb");

    if (old_product_db == NULL) {
        fprintf(stderr, "error: failed to open %s\n", "old_product_db.dat");
        return;
    }

    new_product_db = fopen("product_db.dat", "a");

    if (new_product_db == NULL) {
        fprintf(stderr, "error: failed to open %s\n", "product_db.dat");
        return;
    }

    while (!feof(old_product_db)) {
        fread(&product, sizeof product, 1, old_product_db);

        if (strcmp(name, product.name) != 0) {
            fwrite(&product, sizeof product, 1, new_product_db);
        }
    }

    fclose(old_product_db);
    fclose(new_product_db);

    remove("old_product_db.dat");

    new_page();
    puts("Produit supprimé avec succès");
    getchar();
}

void product_search_by_name(
    const char *name,
    struct product *product,
    bool *exists)
{
    FILE *product_db;
    struct product tmp;

    product_db = fopen("product_db.dat", "rb");

    if (product_db == NULL) {
        fprintf(stderr, "error: failed to open %s\n", "product_db.dat");
        return;
    }

    *exists = false;

    while (!feof(product_db) && (!*exists)) {
        fread(&tmp, sizeof tmp, 1, product_db);
        *exists = (strcmp(tmp.name, name) == 0);
    }

    if (exists && *exists) {
        if (product) {
            *product = tmp;
        }
    } else {
        product = NULL;
        printf("Le produit \"%s\" n'existe pas", name);
        getchar();
    }

    fclose(product_db);
}

void product_search_by_id(
    unsigned short id,
    struct product *product,
    bool *exists)
{
    FILE *product_db;
    struct product tmp;

    product_db = fopen("product_db.dat", "rb");

    if (product_db == NULL) {
        fprintf(stderr, "error: failed to open %s\n", "product_db.dat");
        return;
    }

    *exists = false;

    while (!feof(product_db) && !*exists) {
        fread(&tmp, sizeof tmp, 1, product_db);
        *exists = (tmp.id == id);
    }

    if (exists && *exists) {
        if (product) {
            *product = tmp;
        }
    } else {
        product = NULL;
        printf("Le product n°%hu n'existe pas", id);
        getchar();
    }

    fclose(product_db);
}

static void product_read(struct product *product) {
    puts("Saisie informations produit");

    /* Saisie du nom */
    input_read_last_name(product->name, sizeof product->name);

    /* Saisie de la marque */
    input_read_last_name(product->brand, sizeof product->brand);

    /* Saisie de la provenance */
    input_read_last_name(product->origin, sizeof product->origin);

    /* Saisie du prix */
    input_read_price(&product->price_euro);

    /* Saisie de la masse/volume */
    input_read_price(&product->mass_kg_vol_l);
}
