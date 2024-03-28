#include "supplier.h"

#include <string.h>

static short n_supplier = 0;

static struct supplier supplier_read(void);

void supplier_register(void) {
    FILE *supplier_db;
    struct supplier supplier;

    supplier_db = fopen("supplier_db.dat", "ab");

    if (supplier_db == NULL) {
        fprintf(stderr, "error: failed to open %s\n", "supplier_db.dat");
        return;
    }

    supplier = supplier_read();
    supplier.id = n_supplier++;

    fwrite(&supplier, sizeof supplier, 1, supplier_db);

    fclose(supplier_db);
}

void supplier_inspect(void) {
    struct supplier supplier;
    char name[50];
    bool exists;

    fputs("Nom supplier : ", stdout);
    fgets(name, sizeof name, stdin);
    name[strcspn(name, "\n")] = '\0';

    supplier_search_by_name(name, &supplier, &exists);

    if (exists) {
        puts("supplier informations:");
        printf("supplier: %s\n", supplier.name);
        printf("Phone: %s\n", supplier.phone);
        printf("Email: %s\n", supplier.email);
        printf("Address: %s\n", supplier.address);
    }
}

void supplier_search_by_name( 
    const char *name,
    struct supplier *supplier,
    bool *exists)
{
    FILE *supplier_db;
    struct supplier tmp;

    supplier_db = fopen("supplier_db.dat", "rb");

    if (supplier_db == NULL) {
        fprintf(stderr, "error: failled to open %s\n", "supplier_db.dat");
        return;
    }

    *exists = false;

    while (!feof(supplier_db) && (!*exists)) {
        fread(&tmp, sizeof tmp, 1, supplier_db);
        *exists = (tmp.name == name);
    }

    if (exists) {
        *supplier = tmp;
    } else {
        supplier = NULL;
        printf("Le supplier nommé %s n'existe pas", name);
    }

    fclose(supplier_db);  
}


void supplier_search_by_id(
    const unsigned short id,
    struct supplier *supplier,
    bool *exists)
{
    FILE *supplier_db;
    struct supplier tmp;

    supplier_db = fopen("supplier_db.dat", "rb");

    if (supplier_db == NULL) {
        fprintf(stderr, "error: failed to open %s\n", "supplier_db.dat");
        return;
    }

    *exists = false;

    while (!feof(supplier_db) && !*exists) {
        fread(&tmp, sizeof tmp, 1, supplier_db);
        *exists = (tmp.id == id);
    }

    if (exists) {
        *supplier = tmp;
    } else {
        supplier = NULL;
        printf("Le supplier n°%hd n'existe pas", id);
    }

    fclose(supplier_db);
}


static struct supplier supplier_read(void) {
    struct supplier supplier;

    puts("Enter supplier informations:");

    fputs("Name: ", stdout);
    fgets(supplier.name, sizeof supplier.name, stdin);
    supplier.name[strcspn(supplier.name, "\n")] = '\0';

    fputs("Phone: ", stdout);
    fgets(supplier.phone, sizeof supplier.phone, stdin);
    supplier.phone[strcspn(supplier.phone, "\n")] = '\0';

    fputs("Email: ", stdout);
    fgets(supplier.email, sizeof supplier.email, stdin);
    supplier.email[strcspn(supplier.email, "\n")] = '\0';

    fputs("Address: ", stdout);
    fgets(supplier.address, sizeof supplier.address, stdin);
    supplier.address[strcspn(supplier.address, "\n")] = '\0';

    return supplier;
}
