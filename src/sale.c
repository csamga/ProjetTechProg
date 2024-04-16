#include "sale.h"
#include "client.h"
#include "product.h"
#include "input.h"
#include "terminal.h"
#include "market.h"

#include <stdbool.h>
#include <stdio.h>

static void sale_add_product(FILE *client_db, float *price_tot);

void sale_register(void) {
    struct client client;
    char name[32];
    int choice;
    FILE *client_db, *client_purchase_db;
    float price_tot;
    long pos, end;

    new_page();

    client_db = fopen(CLIENT_DB, "rb");
    end = 0l;

    if (client_db) {
        fseek(client_db, 0l, SEEK_END);
        end = ftell(client_db);
    }

    if (end == 0l) {
        if (client_db != NULL) {
            fclose(client_db);
        }

        log_info("La base de données client est vide");
        return;
    }

    input_read_alpha("Nom client : ", name, sizeof name);
    client_search_by_name(client_db, name, &client, &pos);
    fclose(client_db);

    if (pos == -1l) {
        return;
    }

    client_purchase_db = fopen(client.purchase_db_file_name, "a");

    new_page();

    price_tot = 0.0f;

    do {
        sale_add_product(client_purchase_db, &price_tot);

        puts("Continuer ? (O/n)");
        choice = getchar();
        input_flush_stdin();
    } while (choice == 'o' || choice == 'O' || choice == '\x0d');

    fclose(client_purchase_db);
}

static void sale_add_product(FILE *client_db, float *price_compound) {
    char name[32];
    struct product product;
    short quantity;
    float price_detail;
    FILE *product_db;
    long pos, end;

    product_db = fopen(PRODUCT_DB, "rb");
    end = 0l;

    if (product_db) {
        fseek(product_db, 0l, SEEK_END);
        end = ftell(product_db);
    }

    if ((product_db == NULL) || (end == 0l)) {
        set_text_attr(yellow, true);
        fputs("La base de données produit est vide", stdout);
        reset_text_attr();
        getchar();
        return;
    }

    input_read_alpha("Nom produit : ", name, sizeof name);
    product_search_by_name(product_db, name, &product, &pos);

    if (pos != -1l) {
        fputs(product.name, client_db);
        fputs("Quantité : ", stdout);
        scanf("%hd", &quantity);
        input_flush_stdin();

        price_detail = product.price_euro * (float)quantity;
        *price_compound += price_detail;

        fprintf(
            client_db,
            "\n%d\n%.2f\n%.2f\n\n",
            quantity,
            price_detail,
            *price_compound
        );
    }
}
