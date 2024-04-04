#include "sale.h"
#include "client.h"
#include "product.h"
#include "input.h"
#include "terminal.h"

#include <stdbool.h>
#include <stdio.h>

static void sale_add_product(FILE *client_db, float *price_tot);

void sale_register(void) {
    struct client client;
    char name[32];
    bool exists;
    int choice;
    FILE *client_purchase_db;
    float price_tot;

    new_page();

    input_read_alpha("Nom client : ", name, sizeof name);
    client_search_by_name(name, &client, &exists);

    if (!exists) {
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

    fprintf(client_purchase_db, "%.2f\n\n", price_tot);

    fclose(client_purchase_db);
}

static void sale_add_product(FILE *client_db, float *price_tot) {
    char name[32];
    struct product product;
    bool exists;
    short quantity;

    input_read_alpha("Nom produit : ", name, sizeof name);
    product_search_by_name(name, &product, &exists);

    if (exists) {
        fputs(product.name, client_db);
        fputs("Quantité : ", stdout);
        scanf("%hd", &quantity);
        input_flush_stdin();
        *price_tot += product.price_euro * quantity;
        fprintf(client_db, " %d\n", quantity);
    }
}
