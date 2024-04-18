#include "sale.h"
#include "client.h"
#include "product.h"
#include "input.h"
#include "terminal.h"
#include "market.h"

#include <stdbool.h>
#include <stdio.h>

#define SALES_MARGIN 25.0f/100.0f

static bool sale_add_product(FILE *client_db, FILE *product_db, float *price_compound);

void sale_register(void) {
    struct client client;
    char name[32];
    FILE *client_db, *client_purchase_db, *product_db;
    float price_tot;
    long pos;
    short n_products;
    bool finished;

    log_title("Enregistrement transaction");

    client_db = fopen(CLIENT_DB, "rb");
    product_db = fopen(PRODUCT_DB, "rb+");

    if (!client_db_exists(client_db) || !product_db_exists(product_db)) {
        return;
    }

    input_read_alpha("Nom client : ", name, sizeof name);
    client_search_by_name(client_db, name, &client, &pos);
    fclose(client_db);
    puts("");

    if (pos == -1l) {
        return;
    }

    client_purchase_db = fopen(client.purchase_db_file_name, "a");

    price_tot = 0.0f;
    n_products = 0;

    log_title("Ajout produits");

    do {
        if (sale_add_product(client_purchase_db, product_db, &price_tot)) {
            n_products++;
        }

        finished = !input_confirm("Voulez-vous continuer ?");
        
        if (!finished) {
            puts("");
        }
    } while (!finished);

    fprintf(client_purchase_db, BOLD BLUE "Total dû : %.2f€\n\n" NORMAL, price_tot);

    fclose(client_purchase_db);
    fclose(product_db);

    if (n_products > 0) {
        log_success(
            "Vente enregistrée avec succès\n"
            "Les stocks ont été mis à jour"
        );
    } else {
        log_info(true, "Aucun produit n'a été vendu");
    }
}

static bool sale_add_product(FILE *client_db, FILE *product_db, float *price_compound) {
    char name[32];
    struct product product;
    short quantity;
    float price_detail;
    long pos;
    bool valid;

    input_read_alpha("Nom produit : ", name, sizeof name);
    product_search_by_name(product_db, name, &product, &pos);

    if (pos != -1 && product.quantity <= 0) {
        log_info(true, "Le produit demandé n'est plus en stock");
        return 0;
    }

    if (pos != -1l) {
        do {
            input_read_positive_int(
                "Quantité : ",
                "La quantité doit être positive",
                (int *)&quantity
            );

            valid = quantity != 0;

            if (!valid) {
                log_warning(true, "La quantité ne peut pas être nulle");
                move_cursor_up(1);
                erase_line();
                move_cursor_up(1);
                erase_line();
                continue;
            }

            valid = (quantity <= product.quantity);

            if (!valid) {
                log_warning(
                    true,
                    "La quantité demandée est supérieure au stock disponible (%hu)",
                    product.quantity
                );

                move_cursor_up(1);
                erase_line();
                move_cursor_up(1);
                erase_line();
                continue;
            }
        } while (!valid);

        if (valid) {
            price_detail = product.price_euro * (float)quantity;
            price_detail *= (1 + SALES_MARGIN);
            *price_compound += price_detail;

            fprintf(
                client_db,
                "Produit : %s x %d (%.2f€)\n",
                product.label,
                quantity,
                price_detail
            );

            product.quantity -= quantity;
            fseek(product_db, pos, SEEK_SET);
            fwrite(&product, sizeof product, 1, product_db);

            return valid;
        }
    }

    return 0;
}
