#include "product.h"

#include <stdio.h>
#include <string.h>

static short n_products = 0;

static struct product product_read(void);

void product_register(void) {
    FILE *product_db;
    struct product product;
    
    product_db = fopen("product_db.dat", "ab");

    if (product_db == NULL) {
        fprintf(stderr, "error: failed to open %s\n", "product_db.dat");
        return;
    }

    product = product_read();
    product.id = n_products++;

    fwrite(&product, sizeof product, 1, product_db);

    fclose(product_db);
}

void product_modify(void) {
    FILE *product_db;
    struct product product;
    bool exists;
    char name[50];
    
    product_db = fopen("product_db.dat", "rb+");

    if (product_db == NULL) {
        fprintf(stderr, "error: failed to open %s\n", "product_db.dat");
        return;
    }

    fputs("Nom : ", stdout);
    fgets(name, sizeof name, stdin);
    name[strcspn(name, "\n")] = '\0';

    product_search_by_name(name, &product, &exists);

    if (!exists) {
        return;
    }

    product = product_read();

    fwrite(&product, sizeof product, 1, product_db);

    fclose(product_db);
}

void product_delete(void) {
    FILE *old_product_db, *new_product_db;
    char name[50];
    struct product tmp;
    bool exists;

    fputs("Nom : ", stdout);
    fgets(name, sizeof name, stdin);
    name[strcspn(name, "\n")] = '\0';

    product_search_by_name(name, NULL, &exists);

    if (!exists) {
        return;
    }

    old_product_db = fopen("product_db.dat", "rb");

    if (old_product_db == NULL) {
        fprintf(stderr, "error: failed to open %s\n", "product_db.dat");
        return;
    }

    new_product_db = fopen("tmp_product_db.dat", "a");

    if (new_product_db == NULL) {
        fprintf(stderr, "error: failed to open %s\n", "new_product_db.dat");
        return;
    }

    while (!feof(old_product_db)) {
        fread(&tmp, sizeof tmp, 1, old_product_db);

        if (strcmp(name, tmp.name) != 0) {
            fwrite(&tmp, sizeof tmp, 1, new_product_db);
        }
    }

    fclose(old_product_db);
    fclose(new_product_db);

    remove("product_db.dat");
    rename("tmp_product_db.dat", "product_db.dat");
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

    while (!feof(product_db) && !*exists) {
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

    if (exists && *exists && product) {
        *product = tmp;
    } else {
        product = NULL;
        printf("Le product n°%hd n'existe pas", id);
    }

    fclose(product_db);
}

void product_inspect(void) {
    struct product product;
    char name[50];
    bool exists;

    fputs("Nom produit : ", stdout);
    fgets(name, sizeof name, stdin);
    name[strcspn(name, "\n")] = '\0';

    product_search_by_name(name, &product, &exists);

    if (exists) {
        puts("Product informations:");
        printf("Product: %s\n", product.name);
        printf("Brand: %s\n", product.brand);
        printf("Price: %.2f€\n", product.price_euro);
        printf("Origin: %s\n", product.origin);

        if (product.liquid) {
            printf("Volume: %.3fL\n", product.juan.volume_l);
        } else {
            printf("Mass: %.3fkg\n", product.juan.mass_kg);
        }
    }
}

static struct product product_read(void) {
    struct product product;
    char c;

    puts("Enter product informations:");

    fputs("Name: ", stdout);
    fgets(product.name, sizeof product.name, stdin);
    product.name[strcspn(product.name, "\n")] = '\0';

    fputs("Brand: ", stdout);
    fgets(product.brand, sizeof product.brand, stdin);
    product.brand[strcspn(product.brand, "\n")] = '\0';

    fputs("Price (€): ", stdout);
    scanf("%f", &(product.price_euro));
    getchar();

    fputs("Origin: ", stdout);
    fgets(product.origin, sizeof product.origin, stdin);
    product.origin[strcspn(product.origin, "\n")] = '\0';

    fputs("Liquid (y/n): ", stdout);
    scanf("%c", &c);
    getchar();

    if (c == 'y') {
        product.liquid = true;
        fputs("Volume (L): ", stdout);
        scanf("%f", &(product.juan.volume_l));
        getchar();
    } else {
        product.liquid = false;
        fputs("Mass (kg): ", stdout);
        scanf("%f", &(product.juan.mass_kg));
        getchar();
    }

    puts("");

    return product;
}
