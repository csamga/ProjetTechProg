#include "supplier.h"
#include "input.h"
#include "terminal.h"

#include <string.h>
#include <stdio.h>

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

void supplier_modify(void) {
    FILE *supplier_db;
    struct supplier supplier;
    bool valid;
    char name[32], *tmp;
    size_t len;

    supplier_db = fopen("supplier_db.dat", "rb+");

    if (supplier_db == NULL) {
        fprintf(stderr, "error: failed to open %s\n", "supplier_db.dat");
        return;
    }

    /* Saisie du nom */
    fputs("Nom : ", stdout);

    do {
        input_read_stdin(&tmp, &len);
        valid = input_validate_name(tmp, len);
    } while (!valid);

    strncpy(name, tmp, sizeof name);

    supplier_search_by_name(name, &supplier, &valid);

    free(tmp);

    if (!valid) {
        return;
    }

    supplier_read(&supplier);

    fwrite(&supplier, sizeof supplier, 1, supplier_db);

    fclose(supplier_db);

    puts("Fournisseur modifié avec succès");
    getchar();
}

void supplier_inspect(void) {
    struct supplier supplier;
    char name[32], *tmp;
    size_t len;
    bool valid;

    /* Saisie nom */
    fputs("Nom : ", stdout);
    do {
        input_read_stdin(&tmp, &len);
        valid = input_validate_name(tmp, len);
    } while (!valid);

    supplier_search_by_name(name, &supplier, &valid);

    free(tmp);

    clear_screen();
    set_cursor_home();

    if (valid) {
        puts("Informations fournisseur");
        printf("Identifiant : %hu\n", supplier.id);
        printf("Nom : %s\n", supplier.last_name);
        printf("Prénom : %s\n", supplier.first_name);
        printf("Tel : %s\n", supplier.phone);
        printf("Email : %s\n", supplier.email);
        address_inspect(&supplier.address);

        getchar();
    }
}

void supplier_delete(void) {
    FILE *old_supplier_db, *new_supplier_db;
    struct supplier supplier;
    bool valid;
    char name[32], *tmp;
    size_t len;

    /* Saisie du nom */
    fputs("Nom : ", stdout);

    do {
        input_read_stdin(&tmp, &len);
        valid = input_validate_name(tmp, len);
    } while (!valid);

    strncpy(name, tmp, sizeof name);

    supplier_search_by_name(name, NULL, &valid);

    free(tmp);

    if (!valid) {
        return;
    }

    rename("supplier_db.dat", "old_supplier_db.dat");

    old_supplier_db = fopen("old_supplier_db.dat", "rb");

    if (old_supplier_db == NULL) {
        fprintf(stderr, "error: failed to open %s\n", "old_supplier_db.dat");
        return;
    }

    new_supplier_db = fopen("supplier_db.dat", "a");

    if (new_supplier_db == NULL) {
        fprintf(stderr, "error: failed to open %s\n", "supplier_db.dat");
        return;
    }

    while (!feof(old_supplier_db)) {
        fread(&supplier, sizeof supplier, 1, old_supplier_db);

        if (strcmp(name, supplier.last_name) != 0) {
            fwrite(&supplier, sizeof supplier, 1, new_supplier_db);
        }
    }

    fclose(old_supplier_db);
    fclose(new_supplier_db);

    remove("old_supplier_db.dat");

    puts("Fournisseur supprimé avec succès");
    getchar();
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
        *exists = (strcmp(tmp.last_name, name) == 0);
    }

    if (exists && *exists) {
        if (supplier) {
            *supplier = tmp;
        }
    } else {
        printf("Le supplier \"%s\" n'existe pas", name);
        getchar();
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

    if (exists && *exists) {
        if (supplier) {
            *supplier = tmp;
        }
    } else {
        supplier = NULL;
        printf("Le supplier n°%hu n'existe pas", id);
        getchar();
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
