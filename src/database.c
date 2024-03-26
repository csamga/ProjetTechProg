#include "database.h"

#define MARKET_DB_FILE "market_db.dat"
#define CLIENT_DB_FILE "client_db.dat"
#define PRODUCT_DB_FILE "product_db.dat"

static struct database {
    FILE *market_db;
    FILE *client_db;
    FILE *product_db;
} db;

void database_open(void) {
    db.market_db = fopen(MARKET_DB_FILE, "rb+");
    db.client_db = fopen(CLIENT_DB_FILE, "rb+");
    db.product_db = fopen(PRODUCT_DB_FILE, "rb+");
}

void database_close(void) {
    if (db.market_db) {
        fclose(db.market_db);
    }

    if (db.client_db) {
        fclose(db.client_db);
    }

    if (db.product_db) {
        fclose(db.product_db);
    }
}

FILE *database_get(enum db_type db_type) {
    FILE *db_file;

    switch (db_type) {
    case MARKET:
        db_file = db.market_db;
        break;
    case CLIENT:
        db_file = db.client_db;
        break;
    case PRODUCT:
        db_file = db.product_db;
        break;
    }

    return db_file;
}
