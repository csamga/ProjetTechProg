#include "supplier.h"
#include "input.h"

#include <string.h>

static unsigned short n_supplier = 0;

static void supplier_read(struct supplier *supplier);

void supplier_register(void) {
    FILE *supplier_db;
    struct supplier supplier;

    supplier_db = fopen("supplier_db.dat", "ab");

    if (supplier_db == NULL) {
        fprintf(stderr, "error: failed to open %s\n", "supplier_db.dat");
        return;
    }

    supplier_read(&supplier);
    supplier.id = n_supplier++;

    fwrite(&supplier, sizeof supplier, 1, supplier_db);

    fclose(supplier_db);

    puts("Fournisseur enregistré avec succès");
    getchar();
}

void supplier_inspect(void) {
    struct supplier supplier;
    char name[32];
    bool exists;

    fputs("Nom fournisseur : ", stdout);
    fgets(name, sizeof name, stdin);
    name[strcspn(name, "\n")] = '\0';

    supplier_search_by_name(name, &supplier, &exists);

    if (exists) {
        puts("Informations fournisseur");
        printf("Identifiant : %hu", supplier.id);
        printf("Nom : %s\n", supplier.last_name);
        printf("Prénom : %s\n", supplier.first_name);
        printf("Tel : %s\n", supplier.phone);
        printf("Email : %s\n", supplier.email);
        address_inspect(&supplier.address);

        getchar();
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
        *exists = (strcpy(tmp.last_name, name) == 0);
    }

    if (exists && *exists) {
        if (supplier) {
            *supplier = tmp;
        }
    } else {
        printf("Le supplier \"%s\" n'existe pas", name);
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

static void supplier_read(struct supplier *supplier) {
    char *tmp;
    size_t len;
    bool valid;

    puts("Saisie informations fournisseur");

    /* Saisie du nom */
    fputs("Nom : ", stdout);

    do {
        input_read_stdin(&tmp, &len);
        valid = input_validate_name(tmp, len);
    } while (!valid);

    strncpy(supplier->last_name, tmp, sizeof supplier->last_name);

    /* Saisie du prénom */
    fputs("Prénom : ", stdout);

    do {
        input_read_stdin(&tmp, &len);
        valid = input_validate_name(tmp, len);
    } while (!valid);

    strncpy(supplier->first_name, tmp, sizeof supplier->first_name);

    /* Saisie du numéro de téléphone */
    fputs("Tel : ", stdout);

    do {
        input_read_stdin(&tmp, &len);
        valid = input_validate_phone(tmp, len);
    } while (!valid);

    strncpy(supplier->phone, tmp, sizeof supplier->phone);

    /* Saisie de l'adresse email */
    fputs("Email : ", stdout);

    do {
        input_read_stdin(&tmp, &len);
        valid = input_validate_email(tmp, len);
    } while (!valid);

    strncpy(supplier->email, tmp, sizeof supplier->email);

    /* Saisie de l'adresse */
    address_read(&supplier->address);

    free(tmp);
}
