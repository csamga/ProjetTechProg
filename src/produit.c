#include "produit.h"

#include <stdio.h>
#include <string.h>

static struct product read_product(void);

void register_product(void) {
    FILE *product_db;
    struct product product;
    
    product_db = fopen("product_db.dat", "ab");

    if (product_db == NULL) {
        fprintf(stderr, "error: failed to open %s\n", "product_db.dat");
        return;
    }

    product = read_product();

    fwrite(&product, sizeof product, 1, product_db);

    fclose(product_db);
}

void search_product_by_name(
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

    if (*exists) {
        *product = tmp;
    } else {
        product = NULL;
        printf("Le produit \"%s\" n'existe pas", name);
    }

    fclose(product_db);
}

void search_product_by_id(
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

    if (exists) {
        *product = tmp;
    } else {
        product = NULL;
        printf("Le product n°%hd n'existe pas", id);
    }

    fclose(product_db);
}

void print_product(struct product *product) {
    puts("Product informations:");
    printf("Product: %s\n", product->name);
    printf("Brand: %s\n", product->brand);
    printf("Price: %.2f€\n", product->price_euro);
    printf("Origin: %s\n", product->origin);

    if (product->liquid) {
        printf("Volume: %.3fL\n", product->juan.volume_l);
    } else {
        printf("Mass: %.3fkg\n", product->juan.mass_kg);
    }

    puts("");
}

static struct product read_product(void) {
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

