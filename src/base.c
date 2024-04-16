#include "base.h"

#include "client.h"
#include "supplier.h"
#include "product.h"
#include "terminal.h"

#include <stdio.h>

void base_register(enum modes mode) {
    new_page();

    switch (mode) {
    case mode_client:
        client_register();
        break;
    case mode_supplier:
        supplier_register();
        break;
    case mode_product:
        product_register();
        break;
    case mode_sale:
        puts("implémenter enregistrer transac");
        getchar();
        break;
    case mode_quit:
        break;
    }
}

void base_inspect(enum modes mode) {
    new_page();

    switch (mode) {
    case mode_client:
        client_inspect();
        break;
    case mode_supplier:
        supplier_inspect();
        break;
    case mode_product:
        product_inspect();
        break;
    case mode_sale:
        puts("implémenter consulter histo ventes");
        getchar();
        break;
    case mode_quit:
        break;
    }
}

void base_modify(enum modes mode) {
    new_page();

    switch (mode) {
    case mode_client:
        client_modify();
        break;
    case mode_supplier:
        supplier_modify();
        break;
    case mode_product:
        product_modify();
        break;
    case mode_sale:
        puts("implémenter vente modifier");
        getchar();
        break;
    case mode_quit:
        break;
    }
}

void base_delete(enum modes mode) {
    new_page();

    switch (mode) {
    case mode_client:
        client_delete();
        break;
    case mode_supplier:
        supplier_delete();
        break;
    case mode_product:
        product_delete();
        break;
    case mode_sale:
        puts("implémenter vente supprimer");
        getchar();
        break;
    case mode_quit:
        break;
    }
}
