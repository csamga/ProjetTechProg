#include "sale.h"
#include "client.h"
#include "product.h"
#include "input.h"
#include "terminal.h"

#include <stdbool.h>
#include <stdio.h>

void sale_register(void) {
    struct client client;
    struct product product;
    char name[32];
    bool exists;
    int choice;

    new_page();

    input_read_last_name(name, sizeof name);
    client_search_by_name(name, &client, &exists);

    if (!exists) {
        return;
    }

    new_page();

    do {
        fputs("Produit : ", stdout);

        input_read_last_name(name, sizeof name);
        product_search_by_name(name, &product, &exists);

        if (exists) {
            puts(product.brand);
        }

        puts("Continuer ? (O/n)");
        choice = getchar();
        input_flush_stdin();
    } while (choice == 'o' || choice == 'O' || choice == '\x0d');
}

void sale_add_product(void) {

}
