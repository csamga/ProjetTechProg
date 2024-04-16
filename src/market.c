#include "market.h"

#include <stdio.h>

struct market {
    unsigned short n_clients;
    unsigned short n_suppliers;
    unsigned short n_products;
};

void market_create_db(void) {
    FILE *market_db;
    struct market market;

    market_db = fopen(MARKET_DB, "wbx");

    if (market_db != NULL) {
        market.n_clients = 0;
        market.n_products = 0;
        market.n_suppliers = 0;

        fwrite(&market, sizeof market, 1, market_db);
        fclose(market_db);
    }
}

void market_client_added(void) {
    FILE *market_db;
    struct market market;

    market_db = fopen(MARKET_DB, "rb+");

    if (market_db == NULL) {
        fputs("erreur: le fichier " MARKET_DB " n'existe pas", stderr);
        getchar();
        return;
    }

    fread(&market, sizeof market, 1, market_db);

    market.n_clients++;

    fseek(market_db, 0l, SEEK_SET);
    fwrite(&market, sizeof market, 1, market_db);
    fclose(market_db);
}

void market_supplier_added(void) {
    FILE *market_db;
    struct market market;

    market_db = fopen(MARKET_DB, "rb+");

    if (market_db == NULL) {
        fputs("erreur: le fichier " MARKET_DB " n'existe pas", stderr);
        getchar();
        return;
    }

    fread(&market, sizeof market, 1, market_db);

    market.n_suppliers++;

    fseek(market_db, 0l, SEEK_SET);
    fwrite(&market, sizeof market, 1, market_db);
    fclose(market_db);
}

void market_product_added(void) {
    FILE *market_db;
    struct market market;

    market_db = fopen(MARKET_DB, "rb+");

    if (market_db == NULL) {
        fputs("erreur: le fichier " MARKET_DB " n'existe pas", stderr);
        getchar();
        return;
    }

    fread(&market, sizeof market, 1, market_db);

    market.n_products++;

    fseek(market_db, 0l, SEEK_SET);
    fwrite(&market, sizeof market, 1, market_db);
    fclose(market_db);
}

void market_client_removed(void) {
    FILE *market_db;
    struct market market;

    market_db = fopen(MARKET_DB, "rb+");

    if (market_db == NULL) {
        fputs("erreur: le fichier " MARKET_DB " n'existe pas", stderr);
        getchar();
        return;
    }

    fread(&market, sizeof market, 1, market_db);

    market.n_clients--;

    fseek(market_db, 0l, SEEK_SET);
    fwrite(&market, sizeof market, 1, market_db);
    fclose(market_db);
}

void market_supplier_removed(void) {
    FILE *market_db;
    struct market market;

    market_db = fopen(MARKET_DB, "rb+");

    if (market_db == NULL) {
        fputs("erreur: le fichier " MARKET_DB " n'existe pas", stderr);
        getchar();
        return;
    }

    fread(&market, sizeof market, 1, market_db);

    market.n_suppliers--;

    fseek(market_db, 0l, SEEK_SET);
    fwrite(&market, sizeof market, 1, market_db);
    fclose(market_db);
}

void market_product_removed(void) {
    FILE *market_db;
    struct market market;

    market_db = fopen(MARKET_DB, "rb+");

    if (market_db == NULL) {
        fputs("erreur: le fichier " MARKET_DB " n'existe pas", stderr);
        getchar();
        return;
    }

    fread(&market, sizeof market, 1, market_db);

    market.n_products--;

    fseek(market_db, 0l, SEEK_SET);
    fwrite(&market, sizeof market, 1, market_db);
    fclose(market_db);
}

unsigned short market_get_n_clients(void) {
    FILE *market_db;
    struct market market;
    unsigned short n_clients;

    market_db = fopen(MARKET_DB, "rb");

    if (market_db == NULL) {
        fputs("erreur: le fichier " MARKET_DB " n'existe pas", stderr);
        getchar();
        n_clients = 0;
    } else {
        fread(&market, sizeof market, 1, market_db);
        n_clients = market.n_clients;
    }

    return n_clients;
}

unsigned short market_get_n_suppliers(void) {
    FILE *market_db;
    struct market market;
    unsigned short n_suppliers;

    market_db = fopen(MARKET_DB, "rb");

    if (market_db == NULL) {
        fputs("erreur: le fichier " MARKET_DB " n'existe pas", stderr);
        getchar();
        n_suppliers = 0;
    } else {
        fread(&market, sizeof market, 1, market_db);
        n_suppliers = market.n_suppliers;
    }

    return n_suppliers;
}

unsigned short market_get_n_products(void) {
    FILE *market_db;
    struct market market;
    unsigned short n_products;

    market_db = fopen(MARKET_DB, "rb");

    if (market_db == NULL) {
        fputs("erreur: le fichier " MARKET_DB " n'existe pas", stderr);
        getchar();
        n_products = 0;
    } else {
        fread(&market, sizeof market, 1, market_db);
        n_products = market.n_products;
    }

    return n_products;
}
