#ifndef DATABASE_H_INCLUDED
#define DATABASE_H_INCLUDED

#include <stdio.h>

enum db_type {MARKET, CLIENT, PRODUCT};

void database_open(void);

void database_close(void);

FILE *database_get(enum db_type db_type);

#endif
