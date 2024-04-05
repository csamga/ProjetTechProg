#include "supplier.h"
#include "input.h"
#include "terminal.h"
#include "utils.h"
#include "market.h"

#include <string.h>
#include <stdio.h>

static char *field_str[] = {
    "Nom",
    "Prenom",
    "Tel",
    "Email",
    "Retour"
};

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
    supplier.id = market_get_n_suppliers();
    market_supplier_added();

    fwrite(&supplier, sizeof supplier, 1, supplier_db);
    fclose(supplier_db);

    new_page();
    puts("Fournisseur enregistré avec succès");
    getchar();
}

void supplier_modify(void) {
    FILE *supplier_db;
    struct supplier supplier;
    bool valid, finished;
    char name[32];
    int choice;

    supplier_db = fopen("supplier_db.dat", "rb+");

    if (supplier_db == NULL) {
        fprintf(stderr, "error: failed to open %s\n", "supplier_db.dat");
        return;
    }

    /* Saisie du nom */
    input_read_alpha("Nom : ", name, sizeof name);
    supplier_search_by_name(name, &supplier, &valid);

    if (!valid) {
        return;
    }

    do {
        new_page();
        puts("Champ à modifier");
        list_print(field_str, 5, 1);

        choice = acquire_input();
        finished = false;
        new_page();

        switch (choice) {
        case 0:
            printf("Ancien nom : %s\n", supplier.last_name);
            input_read_alpha("Nouveau nom : ", supplier.last_name, sizeof supplier.last_name);
            break;
        case 1:
            printf("Ancien prénom : %s\n", supplier.first_name);
            input_read_alpha("Nouveau nom : ", supplier.first_name, sizeof supplier.first_name);
            break;
        case 2:
            printf("Ancien tel : %s\n", supplier.phone);
            input_read_num(
                "Nouveau tel : ",
                "Le tel doit contenir exactement 10 chiffres",
                supplier.phone,
                sizeof supplier.phone,
                10
            );
            break;
        case 3:
            printf("Ancien email : %s\n", supplier.email);
            input_read_email("Nouvel email : ", "Adresse email invalide", supplier.email, sizeof supplier.email);
            break;
        case 4:
            finished = true;
        }
    } while (!finished);

    fwrite(&supplier, sizeof supplier, 1, supplier_db);
    fclose(supplier_db);

    new_page();
    puts("Fournisseur modifié avec succès");
    getchar();
}

void supplier_inspect(void) {
    struct supplier supplier;
    char name[32];
    bool valid;

    /* Saisie nom */
    input_read_alpha("Nom : ", name, sizeof name);
    supplier_search_by_name(name, &supplier, &valid);

    if (valid) {
        new_page();

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
    char name[32];

    /* Saisie du nom */
    input_read_alpha("Nom : ", name, sizeof name);
    supplier_search_by_name(name, NULL, &valid);

    if (!valid) {
        return;
    }

    valid = input_confirm_delete("Voulez vous vraiment supprimer le produit ?");

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

    new_page();
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
    puts("Saisie informations fournisseur");

    /* Saisie du nom */
    input_read_alpha("Nom : ", supplier->last_name, sizeof supplier->last_name);

    /* Saisie du prénom */
    input_read_alpha("Prénom : ", supplier->first_name, sizeof supplier->first_name);

    /* Saisie du numéro de téléphone */
    input_read_num(
        "Tel : ",
        "Le tel doit contenir exactement 10 chiffres",
        supplier->phone,
        sizeof supplier->phone,
        10
    );

    /* Saisie de l'adresse email */
    input_read_email("Nouvel email : ", "Adresse email invalide", supplier->email, sizeof supplier->email);

    /* Saisie de l'adresse */
    address_read(&supplier->address);
}
