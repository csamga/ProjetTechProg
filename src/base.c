#include "base.h"

#include "client.h"
#include "supplier.h"
#include "produit.h"

void base_register(enum modes mode) {
    switch (mode) {
    case MODE_CLIENT:
        client_register();
        break;
    case MODE_SUPPLIER:
        supplier_register();
        break;
    case MODE_PRODUCT:
        product_register();
        break;
    case MODE_SALE:
        puts("implémenter enregistrer transac");
        getchar();
        break;
    case MODE_QUIT:
        break;
    }
}

void base_inspect(enum modes mode) {
    switch (mode) {
    case MODE_CLIENT:
        client_inspect();
        break;
    case MODE_SUPPLIER:
        supplier_inspect();
        break;
    case MODE_PRODUCT:
        product_inspect();
        break;
    case MODE_SALE:
        puts("implémenter consulter histo ventes");
        getchar();
        break;
    case MODE_QUIT:
        break;
    }
}
