#define _POSIX_C_SOURCE 200112L
#include "product.h"
#include "input.h"
#include "terminal.h"
#include "utils.h"
#include "market.h"

#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>

static char *field_str[] = {
    "Nom",
    "Marque",
    "Origine",
    "Prix",
    "Masse/volume",
    "Retour"
};

static void product_read(struct product *product);
static void product_print(struct product *product);

void product_register(void) {
    FILE *product_db;
    struct product product;
    
    product_db = fopen(PRODUCT_DB, "ab");

    if (product_db == NULL) {
        log_error("erreur: impossible d'ouvrir " PRODUCT_DB);
        getchar();
        return;
    }

    product.id = market_get_n_products();
    product.quantity = 0;

    product_read(&product);

    fwrite(&product, sizeof product, 1, product_db);
    fclose(product_db);

    market_product_added();

    log_success("Produit enregistré avec succès");
}

void product_modify(void) {
    FILE *product_db;
    struct product product;
    char name[32];
    bool finished;
    int choice;
    long pos, end;

    log_title("Modification produit");
    
    product_db = fopen(PRODUCT_DB, "rb+");
    end = 0l;

    if (product_db) {
        fseek(product_db, 0l, SEEK_END);
        end = ftell(product_db);
    }

    if (end == 0l) {
        if (product_db != NULL) {
            fclose(product_db);
        }

        log_info(true, "La base de données produit est vide");
        return;
    }

    input_read_alpha("Nom : ", name, sizeof name);
    product_search_by_name(product_db, name, &product, &pos);

    if (pos == -1l) {
        return;
    }

    do {
        new_page();

        log_title("Champ à modifier");
        list_print(field_str, COUNTOF(field_str), 1);

        choice = menu_acquire_input();
        finished = false;

        switch (choice) {
        case 0:
            printf("\nAncien nom : %s\n", product.name);
            input_read_alpha("Nouveau nom : ", product.name, sizeof product.name);
            break;
        case 1:
            printf("\nAncienne marque : %s\n", product.brand);
            input_read_alpha("Nouvelle marque : ", product.brand, sizeof product.brand);
            break;
        case 2:
            printf("\nAncienne origine : %s\n", product.origin);
            input_read_alpha("Nouvelle origine : ", product.origin, sizeof product.origin);
            break;
        case 3:
            printf("\nAncien prix : %.2f\n", product.price_euro);
            input_read_positive_float(
                "Nouveau prix : ",
                "Le prix doit être positif",
                &product.price_euro
            );
            break;
        case 4:
            printf("\nAncienne masse/volume (kg/L) : %.3f\n", product.mass_kg_vol_l);
            input_read_positive_float(
                "Nouvelle masse/volume (kg/L) : ",
                "La masse/volume doit être positive",
                &product.mass_kg_vol_l
            );
            break;
        case 5:
            finished = true;
        }
    } while (!finished);

    fseek(product_db, pos, SEEK_SET);
    fwrite(&product, sizeof product, 1, product_db);
    fclose(product_db);

    log_success("Produit modifié avec succès");
}

void product_inspect(void) {
    struct product product;
    char name[32];
    FILE *product_db;
    long pos, end;

    log_title("Informations produit");

    product_db = fopen(PRODUCT_DB, "rb");
    end = 0l;

    if (product_db) {
        fseek(product_db, 0l, SEEK_END);
        end = ftell(product_db);
    }

    if (end == 0l) {
        if (product_db != NULL) {
            fclose(product_db);
        }

        log_info(true, "La base de données produit est vide");
        return;
    }

    input_read_alpha("Nom : ", name, sizeof name);
    product_search_by_name(product_db, name, &product, &pos);

    if (pos != -1l) {
        move_cursor_up(1);
        product_print(&product);
        getchar();
    }

    fclose(product_db);
}

void product_delete(void) {
    FILE *old_product_db, *new_product_db;
    struct product product;
    char name[32];
    bool valid;
    long pos, end;

    log_title("Suppression produit");

    old_product_db = fopen(PRODUCT_DB, "rb+");
    end = 0l;

    if (old_product_db) {
        fseek(old_product_db, 0l, SEEK_END);
        end = ftell(old_product_db);
    }

    if (end == 0l) {
        if (old_product_db != NULL) {
            fclose(old_product_db);
        }

        log_info(true, "La base de données produit est vide");
        return;
    }

    input_read_alpha("Nom : ", name, sizeof name);
    product_search_by_name(old_product_db, name, NULL, &pos);

    if (pos == -1l) {
        return;
    }

    valid = input_confirm("Voulez vous vraiment supprimer le produit ?");

    if (!valid) {
        return;
    }

    new_product_db = fopen("db/tmp.dat", "a");

    if (new_product_db == NULL) {
        log_error("erreur: impossible d'ouvrir db/tmp.dat");
        return;
    }

    while (!feof(old_product_db)) {
        if (fread(&product, sizeof product, 1, old_product_db)) {
            if (strcmp(name, product.name) != 0) {
                fwrite(&product, sizeof product, 1, new_product_db);
            }
        }
    }

    fclose(old_product_db);
    fclose(new_product_db);

    remove(PRODUCT_DB);
    rename("db/tmp.dat", PRODUCT_DB);

    log_success("Produit supprimé avec succès");
}

void product_inspect_inventory(void) {
    FILE *product_db;
    struct product product;
    long end;

    new_page();

    log_title("Inventaire");

    product_db = fopen(PRODUCT_DB, "rb");
    end = 0l;

    if (product_db) {
        fseek(product_db, 0l, SEEK_END);
        end = ftell(product_db);
    }

    if (end == 0l) {
        if (product_db != NULL) {
            fclose(product_db);
        }

        log_info(true, "La base de donées produit est vide");
        return;
    }

    rewind(product_db);

    puts("╭──────────────┬──────────────┬──────────────┬──────────┬─────────────────────┬──────────╮");
    printf(
        "│ %-12s │ %-12s │ %-12s │ %-8s │ %-19s │ %-8s │\n"
        "├──────────────┼──────────────┼──────────────┼──────────┼─────────────────────┼──────────┤\n",
        "Nom",
        "Marque",
        "Origine",
        "Prix (€)",
        "Masse/volume (kg/L)",
        "Quantité"
    );

    while (!feof(product_db)) {
        if (fread(&product, sizeof product, 1, product_db)) {
            printf(
                "│ %-12.12s │ %-12.12s │ %-12.12s │ %-8.2f │ %-19.3f │ %8hd │\n"
                "├──────────────┼──────────────┼──────────────┼──────────┼─────────────────────┼──────────┤\n",
                product.name,
                product.brand,
                product.origin,
                product.price_euro,
                product.mass_kg_vol_l,
                product.quantity
            );
        }
    }

    move_cursor_up(1);
    erase_line();
    puts( "╰──────────────┴──────────────┴──────────────┴──────────┴─────────────────────┴──────────╯");

    fclose(product_db);

    getchar();
}

void product_search_by_name(
    FILE *product_db,
    const char *name,
    struct product *product,
    long *pos)
{
    struct product tmp;
    bool exists;

    rewind(product_db);

    exists = false;

    while (!feof(product_db) && (!exists)) {
        fread(&tmp, sizeof tmp, 1, product_db);
        exists = (strcmp(tmp.name, name) == 0);
    }

    if (exists) {
        if (product) {
            *product = tmp;
        }
        if (pos) {
            *pos = ftell(product_db) - sizeof tmp;
        }
    } else {
        if (pos) {
            *pos = -1l;
        }

        log_info(true, "Le produit \"%s\" n'existe pas", name);
    }
}

void product_search_by_id(
    FILE *product_db,
    unsigned short id,
    struct product *product,
    long *pos)
{
    struct product tmp;
    bool exists;

    rewind(product_db);

    exists = false;

    while (!feof(product_db) && !exists) {
        if (fread(&tmp, sizeof tmp, 1, product_db)) {
            exists = (tmp.id == id);
        }
    }

    if (exists) {
        if (product) {
            *product = tmp;
        }

        if (pos) {
            *pos = ftell(product_db) - sizeof tmp;
        }
    } else {
        if (pos) {
            *pos = -1l;
        }

        log_info(true, "Le produit n°%hu n'existe pas", id);
    }
}

bool product_db_exists(FILE *product_db) {
    long pos;

    pos = 0l;

    if (product_db) {
        fseek(product_db, 0l, SEEK_END);
        pos = ftell(product_db);
    }

    if (pos == 0l) {
        if (product_db != NULL) {
            fclose(product_db);
        }

        log_info(true, "La base de données produit est vide");
    }

    return (pos != 0l);
}

static void product_read(struct product *product) {
    log_title("Saisir informations produit");

    /* Saisie du nom */
    input_read_alpha("Nom : ", product->name, sizeof product->name);

    /* Saisie de la marque */
    input_read_alpha("Marque : ", product->brand, sizeof product->brand);

    /* Saisie de la provenance */
    input_read_alpha("Provenance : ", product->origin, sizeof product->origin);

    /* Saisie du prix */
    input_read_positive_float(
        "Prix : ",
        "Le prix doit être positif",
        &product->price_euro
    );

    /* Saisie de la masse/volume */
    input_read_positive_float(
        "Masse/volume (kg/L) : ",
        "La masse/volume doit être positive",
        &product->mass_kg_vol_l
    );
}

static void product_print(struct product *product) {
    printf("Identifiant : %hu\n", product->id);
    printf("Nom : %s\n", product->name);
    printf("Marque : %s\n", product->brand);
    printf("Origine : %s\n", product->origin);
    printf("Prix : %.2f€\n", product->price_euro);
    printf("Masse/volume (kg/L) : %.3f\n", product->mass_kg_vol_l);
    printf("Quantité : %hd\n", product->quantity);
}
