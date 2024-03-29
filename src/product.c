#include "product.h"
#include "input.h"
#include "terminal.h"

#include <stdio.h>
#include <string.h>

static unsigned short n_products = 0;

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
    product.id = n_products++;

    fwrite(&product, sizeof product, 1, product_db);

    fclose(product_db);

    puts("Produit enregistré avec succès");
    getchar();
}

void product_modify(void) {
    FILE *product_db;
    struct product product;
    char name[32], *tmp;
    size_t len;
    bool valid;
    
    product_db = fopen("product_db.dat", "rb+");

    if (product_db == NULL) {
        fprintf(stderr, "error: failed to open %s\n", "product_db.dat");
        return;
    }

    /* Saisie du nom */
    fputs("Nom : ", stdout);

    do {
        input_read_stdin(&tmp, &len);
        valid = input_validate_name(tmp, len);
    } while (!valid);

    strncpy(name, tmp, sizeof name);

    product_search_by_name(name, &product, &valid);

    free(tmp);

    if (!valid) {
        return;
    }

    product_read(&product);

    fwrite(&product, sizeof product, 1, product_db);

    fclose(product_db);

    puts("Produit modifié avec succès");
    getchar();
}

void product_inspect(void) {
    struct product product;
    char name[32], *tmp;
    size_t len;
    bool valid;

    /* Saisie du nom */
    fputs("Nom : ", stdout);

    do {
        input_read_stdin(&tmp, &len);
        valid = input_validate_name(tmp, len);
    } while (!valid);

    strncpy(name, tmp, sizeof name);

    product_search_by_name(name, &product, &valid);

    free(tmp);

    clear_screen();
    set_cursor_home();

    if (valid) {
        puts("Informations produit");
        printf("Identifiant: %hu\n", product.id);
        printf("Nom: %s\n", product.name);
        printf("Marque: %s\n", product.brand);
        printf("Origine: %s\n", product.origin);
        printf("Prix: %.2f€\n", product.price_euro);

        if (product.liquid) {
            printf("Volume: %.3fL\n", product.juan.volume_l);
        } else {
            printf("Masse: %.3fkg\n", product.juan.mass_kg);
        }

        getchar();
    }
}

void product_delete(void) {
    FILE *old_product_db, *new_product_db;
    struct product product;
    char name[32], *tmp;
    size_t len;
    bool valid;

    /* Saisie du nom */
    fputs("Nom : ", stdout);

    do {
        input_read_stdin(&tmp, &len);
        valid = input_validate_name(tmp, len);
    } while (!valid);

    strncpy(name, tmp, sizeof name);

    product_search_by_name(name, NULL, &valid);

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

    if (exists && *exists && product) {
        *product = tmp;
    } else {
        product = NULL;
        printf("Le product n°%hd n'existe pas", id);
        getchar();
    }

    fclose(product_db);
}

static void product_read(struct product *product) {
    char *tmp, c;
    size_t len;
    bool valid;

    puts("Saisie informations produit");

    /* Saisie du nom */
    fputs("Nom : ", stdout);

    do {
        input_read_stdin(&tmp, &len);
        valid = input_validate_name(tmp, len);
    } while (!valid);

    strncpy(product->name, tmp, sizeof product->name);

    /* Saisie de la marque */
    fputs("Marque : ", stdout);

    do {
        input_read_stdin(&tmp, &len);
        valid = input_validate_name(tmp, len);
    } while (!valid);

    strncpy(product->brand, tmp, sizeof product->brand);

    /* Saisie de la provenance */
    fputs("Origine : ", stdout);

    do {
        input_read_stdin(&tmp, &len);
        valid = input_validate_name(tmp, len);
    } while (!valid);

    strncpy(product->origin, tmp, sizeof product->origin);

    /* Saisie du prix */
    fputs("Prix (€) : ", stdout);
    scanf("%f", &(product->price_euro));
    getchar();

    /* Saisie du type (liquide/solide) */

    fputs("Liquide (y/n) : ", stdout);
    scanf("%c", &c);
    getchar();

    if (c == 'y') {
        product->liquid = true;
        fputs("Volume (L) : ", stdout);
        scanf("%f", &(product->juan.volume_l));
        getchar();
    } else {
        product->liquid = false;
        fputs("Masse (kg) : ", stdout);
        scanf("%f", &(product->juan.mass_kg));
        getchar();
    }

    free(tmp);
}
