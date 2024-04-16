#ifndef MARKET_H_INCLUDED
#define MARKET_H_INCLUDED

#define MARKET_DB "db/market_db.dat"
#define CLIENT_DB "db/client_db.dat"
#define SUPPLIER_DB "db/supplier_db.dat"
#define PRODUCT_DB "db/product_db.dat"

#define PER_CLIENT_DB_PREFIX "db/clients/client_"
#define PER_CLIENT_DB_SUFFIX "_db.dat"

#define PER_SUPPLIER_DB_PREFIX "db/suppliers/supplier_"
#define PER_SUPPLIER_DB_SUFFIX "_db.dat"

void market_create_db(void);

void market_client_added(void);
void market_supplier_added(void);
void market_product_added(void);

void market_client_removed(void);
void market_supplier_removed(void);
void market_product_removed(void);

unsigned short market_get_n_clients(void);
unsigned short market_get_n_suppliers(void);
unsigned short market_get_n_products(void);

#endif
