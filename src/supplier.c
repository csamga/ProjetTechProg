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
static void supplier_create_db(struct supplier *supplier);

void supplier_register(void) {
    FILE *supplier_db;
    struct supplier supplier;

    supplier_db = fopen(SUPPLIER_DB, "ab");

    if (supplier_db == NULL) {
        fputs("erreur: impossible d'ouvrir" SUPPLIER_DB, stderr);
        getchar();
        return;
    }

    supplier.id = market_get_n_suppliers();
    supplier.n_order = 0;
    supplier_create_db(&supplier);
    supplier_read(&supplier);

    fwrite(&supplier, sizeof supplier, 1, supplier_db);
    fclose(supplier_db);

    market_supplier_added();

    new_page();
    puts("Fournisseur enregistré avec succès");
    getchar();
}

void supplier_modify(void) {
    FILE *supplier_db;
    struct supplier supplier;
    bool finished;
    char name[32];
    int choice;
    long pos;

    supplier_db = fopen(SUPPLIER_DB, "rb+");

    if (supplier_db == NULL) {
        fputs("erreur: impossible d'ouvrir" SUPPLIER_DB, stderr);
        getchar();
        return;
    }

    input_read_alpha("Nom : ", name, sizeof name);
    supplier_search_by_name(supplier_db, name, &supplier, &pos);

    if (pos == -1l) {
        return;
    }

    do {
        new_page();
        puts("Champ à modifier");
        list_print(field_str, 5, 1);

        choice = menu_acquire_input();
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

    fseek(supplier_db, pos, SEEK_SET);
    fwrite(&supplier, sizeof supplier, 1, supplier_db);
    fclose(supplier_db);

    new_page();
    puts("Fournisseur modifié avec succès");
    getchar();
}

void supplier_inspect(void) {
    FILE *supplier_db;
    struct supplier supplier;
    char name[32];
    long pos;

    supplier_db = fopen(SUPPLIER_DB, "rb");

    input_read_alpha("Nom : ", name, sizeof name);
    supplier_search_by_name(supplier_db, name, &supplier, &pos);

    if (pos != -1l) {
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

    fclose(supplier_db);
}

void supplier_delete(void) {
    FILE *supplier_db, *new_supplier_db;
    struct supplier supplier;
    char name[32];
    long pos;

    supplier_db = fopen(SUPPLIER_DB, "rb");

    if (supplier_db == NULL) {
        fputs("erreur: impossible d'ouvrir " SUPPLIER_DB, stderr);
        getchar();
        return;
    }

    input_read_alpha("Nom : ", name, sizeof name);
    supplier_search_by_name(supplier_db, name, &supplier, &pos);

    if (pos == -1l) {
        return;
    }

    if (!input_confirm_delete("Voulez vous vraiment supprimer le produit ?")) {
        return;
    }

    new_supplier_db = fopen("db/new_supplier_db.dat", "wb");

    if (new_supplier_db == NULL) {
        fputs("erreur: impossible d'ouvrir db/new_supplier_db.dat", stderr);
        getchar();
        return;
    }

    rewind(supplier_db);

    while (!feof(supplier_db)) {
        fread(&supplier, sizeof supplier, 1, supplier_db);

        if (strcmp(name, supplier.last_name) != 0) {
            fwrite(&supplier, sizeof supplier, 1, new_supplier_db);
        }
    }

    fclose(supplier_db);
    remove(SUPPLIER_DB);

    fclose(new_supplier_db);
    rename("db/new_supplier_db.dat", SUPPLIER_DB);

    remove(supplier.order_db_file_name);
    market_supplier_removed();

    new_page();
    puts("Fournisseur supprimé avec succès");
    getchar();
}

void order_add_product(struct order *order) {
    char name[32];
    struct product product;
    float price_detail;
    FILE *product_db;
    long pos, end;

    product_db = fopen(PRODUCT_DB, "rb");
    end = 0l;

    if (product_db) {
        fseek(product_db, 0l, SEEK_END);
        end = ftell(product_db);
    }

    if ((product_db == NULL) || (end == 0l)) {
        set_text_attr(yellow, true);
        fputs("La base de données produit est vide", stdout);
        reset_text_attr();
        getchar();
        return;
    }

    input_read_alpha("Nom produit : ", name, sizeof name);
    product_search_by_name(product_db, name, &product, &pos);

    if (pos != -1l) {
        order->product_id[order->n_products] = product.id;
        fputs("Quantité : ", stdout);
        scanf("%hu", &order->quantity[order->n_products]);
        input_flush_stdin();

        price_detail = product.price_euro * (float)order->quantity[order->n_products];
        order->total_euro += price_detail;
        order->n_products++;
    }
}

void supplier_order_added(FILE *supplier_db, char *name) {
    struct supplier supplier;
    long pos;

    supplier_search_by_name(supplier_db, name, &supplier, &pos);

    if (pos != -1l) {
        supplier.n_order++;
        fseek(supplier_db, pos, SEEK_SET);
        fwrite(&supplier, sizeof supplier, 1, supplier_db);
    }
}

void supplier_place_order(void) {
    FILE *supplier_db;
    struct supplier supplier;
    char name[32];
    FILE *supplier_order_db;
    int choice;
    struct order order;
    long pos;

    supplier_db = fopen(SUPPLIER_DB, "rb+");

    input_read_alpha("Nom du fournisseur : ", name, sizeof name);
    supplier_search_by_name(supplier_db, name, &supplier, &pos);

    if (pos == -1l) {
        return;
    }

    supplier_order_db = fopen(supplier.order_db_file_name, "ab");

    if (supplier_order_db == NULL) {
        fprintf(stderr, "erreur: impossible d'ouvrir %s\n", supplier.order_db_file_name);
        getchar();
        return;
    }

    order.n_products = 0;
    order.order_id = supplier.n_order;
    order.total_euro = 0.0f;

    do {
        order_add_product(&order);

        puts("Continuer ? (O/n)");
        choice = getchar();
        input_flush_stdin();
    } while (
        (order.n_products < MAX_PRODUCTS_PER_COMMAND) &&
        (choice == 'o' || choice == 'O' || choice == '\x0d')
    );

    fwrite(&order, sizeof order, 1, supplier_order_db);

    fclose(supplier_order_db);

    supplier_order_added(supplier_db, name);
    fclose(supplier_db);
}

void supplier_inspect_order(void) {
    FILE *supplier_db, *supplier_order_db, *product_db;
    struct supplier supplier;
    unsigned short order_id, product_id;
    bool found;
    char name[32];
    struct order order;
    unsigned short i;
    struct product product;
    long end, pos;

    supplier_db = fopen(SUPPLIER_DB, "rb");

    input_read_alpha("Nom du fournisseur : ", name, sizeof name);
    supplier_search_by_name(supplier_db, name, &supplier, &pos);

    fclose(supplier_db);

    if (pos == -1l) {
        return;
    }

    supplier_order_db = fopen(supplier.order_db_file_name, "rb");

    fseek(supplier_order_db, 0l, SEEK_END);
    end = ftell(supplier_order_db);

    if (end == 0l) {
        puts("Ce fournisseur n'a aucune commande en attente de livraison");
        getchar();
        return;
    }

    rewind(supplier_order_db);

    input_read_positive_int(
        "Identifiant commande : ",
        "L'identifiant doit être positif",
        (int *)&order_id
    );

    while (!feof(supplier_order_db) && !found) {
        fread(&order, sizeof order, 1, supplier_order_db);
        found = (order.order_id == order_id);
    }

    if (!found) {
        return;
    }

    fclose(supplier_order_db);

    product_db = fopen(PRODUCT_DB, "rb");

    for (i = 0; i < order.n_products; i++) {
        product_id = order.product_id[i];
        product_search_by_id(product_db, product_id, &product, &pos);

        if (pos != -1l) {
            printf("Nom : %s\n", product.name);
            printf("Quantité : %hu\n", order.quantity[i]);
        }
    }

    fclose(product_db);

    printf("Total dû (€): %.2f\n", order.total_euro);
    getchar();
}

void supplier_register_delivery(void) {
    FILE *supplier_db, *supplier_order_db, *supplier_order_db_tmp, *product_db;
    struct supplier supplier;
    char name[32];
    long pos;
    unsigned short order_id, i, product_id;
    bool found;
    struct order order, order_tmp;
    struct product product;

    supplier_db = fopen(SUPPLIER_DB, "rb");
    input_read_alpha("Nom : ", name, sizeof name);
    supplier_search_by_name(supplier_db, name, &supplier, &pos);
    fclose(supplier_db);

    if (pos == -1l) {
        return;
    }

    supplier_order_db = fopen(supplier.order_db_file_name, "rb+");

    if (supplier_order_db == NULL) {
        fputs("pas de commandes", stdout);
        getchar();
        return;
    }

    input_read_positive_int(
        "Identifiant commande : ",
        "L'identifiant doit être positif",
        (int *)&order_id
    );

    while (!feof(supplier_order_db) && !found) {
        fread(&order, sizeof order, 1, supplier_order_db);
        found = (order.order_id == order_id);
    }

    if (!found) {
        return;
    } else {
        rewind(supplier_order_db);
        supplier_order_db_tmp = fopen("db/tmp.dat", "wb");

        while (!feof(supplier_order_db)) {
            fread(&order_tmp, sizeof order_tmp, 1, supplier_order_db);

            if (order_tmp.order_id != order_id) {
                fwrite(&order_tmp, sizeof order_tmp, 1, supplier_order_db_tmp);
            }
        }

        fclose(supplier_order_db);
        fclose(supplier_order_db_tmp);
        remove(supplier.order_db_file_name);
        rename("db/tmp.dat", supplier.order_db_file_name);
    }

    product_db = fopen(PRODUCT_DB, "rb+");

    for (i = 0; i < order.n_products; i++) {
        product_id = order.product_id[i];
        product_search_by_id(product_db, product_id, &product, &pos);

        product.quantity += order.quantity[i];
        fseek(product_db, pos, SEEK_SET);
        fwrite(&product, sizeof product, 1, product_db);
    }

    fclose(product_db);
}

void supplier_search_by_name(
    FILE *supplier_db,
    const char *name,
    struct supplier *supplier,
    long *pos)
{
    struct supplier tmp;
    bool exists;

    rewind(supplier_db);

    exists = false;

    while (!feof(supplier_db) && !exists) {
        fread(&tmp, sizeof tmp, 1, supplier_db);
        exists = (strcmp(tmp.last_name, name) == 0);
    }

    if (exists) {
        if (supplier) {
            *supplier = tmp;
        }
        if (pos) {
            *pos = ftell(supplier_db) - sizeof tmp;
        }
    } else {
        if (pos) {
            *pos = -1l;
        }

        printf("Le fournisseur \"%s\" n'existe pas", name);
        getchar();
    }
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
    input_read_email("Email : ", "Adresse email invalide", supplier->email, sizeof supplier->email);

    /* Saisie de l'adresse */
    address_read(&supplier->address);
}

static void supplier_create_db(struct supplier *supplier) {
    FILE *db;
    char tmp[64], num_str[8];

    strncpy(tmp, PER_SUPPLIER_DB_PREFIX, sizeof tmp);
    sprintf(num_str, "%d", supplier->id);
    strncat(tmp, num_str, sizeof tmp - strlen(num_str));
    strncat(tmp, PER_SUPPLIER_DB_SUFFIX, sizeof tmp - strlen(PER_SUPPLIER_DB_SUFFIX));
    strncpy(supplier->order_db_file_name, tmp, sizeof supplier->order_db_file_name);

    db = fopen(supplier->order_db_file_name, "wx");

    if (db == NULL) {
        fprintf(stderr, "erreur: impossible de créer le fichier %s", tmp);
        getchar();
        return;
    }

    fclose(db);
}
