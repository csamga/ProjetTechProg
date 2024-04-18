#include "supplier.h"
#include "input.h"
#include "terminal.h"
#include "utils.h"
#include "market.h"
#include "product.h"

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
static void supplier_print(struct supplier *supplier);
static bool supplier_db_exists(FILE *supplier_db);

static bool order_add_product(FILE *product_db, struct order *order);
static void supplier_order_added(FILE *supplier_db, char *name);

void supplier_register(void) {
    FILE *supplier_db;
    struct supplier supplier;

    supplier_db = fopen(SUPPLIER_DB, "ab");

    if (supplier_db == NULL) {
        log_error("erreur: impossible d'ouvrir" SUPPLIER_DB);
        return;
    }

    supplier.id = market_get_n_suppliers();
    supplier.n_order = 0;

    supplier_read(&supplier);
    supplier_create_db(&supplier);

    fwrite(&supplier, sizeof supplier, 1, supplier_db);
    fclose(supplier_db);

    market_supplier_added();

    log_success("Fournisseur enregistré avec succès");
}

void supplier_modify(void) {
    FILE *supplier_db;
    struct supplier supplier;
    bool finished;
    char name[32];
    int choice;
    long pos;

    log_title("Modification fournisseur");

    supplier_db = fopen(SUPPLIER_DB, "rb+");

    if (!supplier_db_exists(supplier_db)) {
        return;
    }

    input_read_alpha("Nom : ", name, sizeof name);
    supplier_search_by_name(supplier_db, name, &supplier, &pos);

    if (pos == -1l) {
        return;
    }

    do {
        new_page();

        log_title("Champ à modifier");
        list_print(field_str, 5, 1);

        choice = menu_acquire_input();
        finished = false;

        switch (choice) {
        case 0:
            printf("\nAncien nom : %s\n", supplier.last_name);
            input_read_alpha("Nouveau nom : ", supplier.last_name, sizeof supplier.last_name);
            break;
        case 1:
            printf("\nAncien prénom : %s\n", supplier.first_name);
            input_read_alpha("Nouveau nom : ", supplier.first_name, sizeof supplier.first_name);
            break;
        case 2:
            printf("\nAncien tel : %s\n", supplier.phone);
            input_read_num(
                "Nouveau tel : ",
                "Le tel doit contenir exactement 10 chiffres",
                supplier.phone,
                sizeof supplier.phone,
                10
            );
            break;
        case 3:
            printf("\nAncien email : %s\n", supplier.email);
            input_read_email("Nouvel email : ", "Adresse email invalide", supplier.email, sizeof supplier.email);
            break;
        case 4:
            finished = true;
        }
    } while (!finished);

    fseek(supplier_db, pos, SEEK_SET);
    fwrite(&supplier, sizeof supplier, 1, supplier_db);
    fclose(supplier_db);

    log_success("Fournisseur modifié avec succès");
}

void supplier_inspect(void) {
    FILE *supplier_db;
    struct supplier supplier;
    char name[32];
    long pos;

    log_title("Informations fournisseur");

    supplier_db = fopen(SUPPLIER_DB, "rb");

    if (!supplier_db_exists(supplier_db)) {
        return;
    }

    input_read_alpha("Nom : ", name, sizeof name);
    supplier_search_by_name(supplier_db, name, &supplier, &pos);

    if (pos != -1l) {
        move_cursor_up(1);
        supplier_print(&supplier);
        getchar();
    }

    fclose(supplier_db);
}

void supplier_delete(void) {
    FILE *supplier_db, *new_supplier_db;
    struct supplier supplier;
    char name[32];
    long pos;

    log_title("Suppression fournisseur");

    supplier_db = fopen(SUPPLIER_DB, "rb");

    if (!supplier_db_exists(supplier_db)) {
        return;
    }

    input_read_alpha("Nom : ", name, sizeof name);
    supplier_search_by_name(supplier_db, name, &supplier, &pos);

    if (pos == -1l) {
        return;
    }

    if (!input_confirm_delete("Voulez vous vraiment supprimer le fournisseur ?")) {
        return;
    }

    new_supplier_db = fopen("db/tmp.dat", "ab");

    if (new_supplier_db == NULL) {
        log_error("erreur: impossible d'ouvrir db/tmp.dat");
        return;
    }

    while (!feof(supplier_db)) {
        if (fread(&supplier, sizeof supplier, 1, supplier_db)) {
            if (strcmp(name, supplier.last_name) != 0) {
                fwrite(&supplier, sizeof supplier, 1, new_supplier_db);
            }
        }
    }

    fclose(supplier_db);
    remove(SUPPLIER_DB);

    fclose(new_supplier_db);
    rename("db/tmp.dat", SUPPLIER_DB);

    remove(supplier.order_db_file_name);
    market_supplier_removed();

    log_success("Fournisseur supprimé avec succès");
}

void supplier_place_order(void) {
    FILE *supplier_db, *product_db;
    struct supplier supplier;
    char name[32];
    FILE *supplier_order_db;
    int choice;
    struct order order;
    long pos;
    bool finished;

    new_page();
    log_title("Création commande");

    supplier_db = fopen(SUPPLIER_DB, "rb+");

    if (!supplier_db_exists(supplier_db)) {
        return;
    }

    product_db = fopen(PRODUCT_DB, "rb");

    if (!product_db_exists(product_db)) {
        return;
    }

    input_read_alpha("Nom du fournisseur : ", name, sizeof name);
    supplier_search_by_name(supplier_db, name, &supplier, &pos);

    if (pos == -1l) {
        return;
    }

    supplier_order_db = fopen(supplier.order_db_file_name, "ab");

    if (supplier_order_db == NULL) {
        log_error("erreur: impossible d'ouvrir %s\n", supplier.order_db_file_name);
        return;
    }

    order.order_id = supplier.n_order;
    order.n_products = 0;
    order.total_euro = 0.0f;

    do {
        order_add_product(product_db, &order);

        finished = (order.n_products >= MAX_PRODUCTS_PER_COMMAND);

        if (finished) {
            log_info(false, "Le nombre maximal de produits par commande est atteint\n");
            continue;
        } else {
            fputs("Continuer ? [O/n] ", stdout);
            choice = getchar();
            input_flush_stdin();

            finished = !(choice == 'o' || choice == 'O' || choice == '\x0d');
        }
    } while (!finished);

    fclose(product_db);

    if (order.n_products > 0) {
        fwrite(&order, sizeof order, 1, supplier_order_db);
        fclose(supplier_order_db);

        supplier_order_added(supplier_db, name);

        log_success("Commande enregistrée avec succès");
    } else {
        log_info(true, "Aucun article n'a été commandé");
    }
    fclose(supplier_db);
}

void supplier_inspect_order(void) {
    FILE *supplier_db, *supplier_order_db, *product_db;
    struct supplier supplier;
    struct product product;
    struct order order;
    char name[32];
    unsigned short product_id, i_order, i_product;
    long pos;
    float product_tot;

    new_page();
    log_title("Informations commandes");

    supplier_db = fopen(SUPPLIER_DB, "rb");

    if (!supplier_db_exists(supplier_db)) {
        return;
    }

    input_read_alpha("Nom du fournisseur : ", name, sizeof name);
    supplier_search_by_name(supplier_db, name, &supplier, &pos);

    fclose(supplier_db);

    if (pos == -1l) {
        return;
    }

    supplier_order_db = fopen(supplier.order_db_file_name, "rb");

    fseek(supplier_order_db, 0l, SEEK_END);
    pos = ftell(supplier_order_db);
    rewind(supplier_order_db);

    if (pos == 0l) {
        log_info(true, "Le fournisseur n'a aucune commande en attente de livraison");
        return;
    }

    product_db = fopen(PRODUCT_DB, "rb");

    i_order = 0;

    while (!feof(supplier_order_db)) {
        if (fread(&order, sizeof order, 1, supplier_order_db)) {
            log_title("\nCommande %hu (ID %hu)", ++i_order, order.order_id);

            for (i_product = 0; i_product < order.n_products; i_product++) {
                product_id = order.product_id[i_product];
                product_search_by_id(product_db, product_id, &product, &pos);

                if (pos != -1l) {
                    product_tot = product.price_euro * order.quantity[i_product];
                    printf(
                        "Produit : %s x %hu (%.2f€)\n",
                        product.name, order.quantity[i_product], product_tot
                    );
                }
            }

            log_info(false, "Total dû (€) : %.2f\n", order.total_euro);
        }
    }

    fclose(supplier_order_db);
    fclose(product_db);

    getchar();
}

void supplier_register_delivery(void) {
    FILE *supplier_db, *supplier_order_db, *supplier_order_db_tmp, *product_db;
    struct supplier supplier;
    struct order order, order_tmp;
    struct product product;
    char name[32];
    long pos;
    unsigned short order_id, i, product_id;
    bool found;

    new_page();
    log_title("Enregistrement livraison");

    supplier_db = fopen(SUPPLIER_DB, "rb");

    if (!supplier_db_exists(supplier_db)) {
        return;
    }

    input_read_alpha("Nom : ", name, sizeof name);
    supplier_search_by_name(supplier_db, name, &supplier, &pos);
    fclose(supplier_db);

    if (pos == -1l) {
        return;
    }

    supplier_order_db = fopen(supplier.order_db_file_name, "rb+");

    fseek(supplier_order_db, 0l, SEEK_END);
    pos = ftell(supplier_order_db);
    rewind(supplier_order_db);

    if (pos == 0l) {
        log_info(true, "Le fournisseur n'a aucune commande en attente de livraison");
        return;
    }

    do {
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
            log_info(false, "Aucune commande ne possède l'identifiant %hu\n", order_id);

            if (input_confirm_delete("Voulez-vous consulter les commandes du fournisseur ?")) {
                supplier_inspect_order();
                new_page();
                log_title("Enregistrement livraison");
                rewind(supplier_order_db);
            } else {
                fclose(supplier_order_db);
                return;
            }
        }
    } while (!found);

    rewind(supplier_order_db);
    supplier_order_db_tmp = fopen("db/tmp.dat", "ab");

    while (!feof(supplier_order_db)) {
        if (fread(&order_tmp, sizeof order_tmp, 1, supplier_order_db)) {
            if (order_tmp.order_id != order_id) {
                fwrite(&order_tmp, sizeof order_tmp, 1, supplier_order_db_tmp);
            }
        }
    }

    fclose(supplier_order_db);
    fclose(supplier_order_db_tmp);
    remove(supplier.order_db_file_name);
    rename("db/tmp.dat", supplier.order_db_file_name);

    product_db = fopen(PRODUCT_DB, "rb+");

    for (i = 0; i < order.n_products; i++) {
        product_id = order.product_id[i];
        product_search_by_id(product_db, product_id, &product, &pos);

        product.quantity += order.quantity[i];
        fseek(product_db, pos, SEEK_SET);
        fwrite(&product, sizeof product, 1, product_db);
    }

    fclose(product_db);

    log_success(
        "Livraison enregistrée avec succès\n"
        "Les stocks ont été mis à jour"
    );
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

        log_info(true, "Le fournisseur \"%s\" n'existe pas", name);
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
        log_info(true, "Le supplier n°%hu n'existe pas", id);
    }

    fclose(supplier_db);
}

static void supplier_read(struct supplier *supplier) {
    log_title("Saisie informations fournisseur");

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
        log_error("erreur: le fichier %s existe déjà", tmp);
        return;
    }

    fclose(db);
}

static void supplier_print(struct supplier *supplier) {
    printf("Identifiant : %hu\n", supplier->id);
    printf("Nom : %s\n", supplier->last_name);
    printf("Prénom : %s\n", supplier->first_name);
    printf("Tel : %s\n", supplier->phone);
    printf("Email : %s\n", supplier->email);
    address_inspect(&supplier->address);
}

static bool order_add_product(FILE *product_db, struct order *order) {
    char name[32];
    struct product product;
    float price_detail;
    long pos;

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

        return true;
    } else {
        return false;
    }
}

static void supplier_order_added(FILE *supplier_db, char *name) {
    struct supplier supplier;
    long pos;

    supplier_search_by_name(supplier_db, name, &supplier, &pos);

    if (pos != -1l) {
        supplier.n_order++;
        fseek(supplier_db, pos, SEEK_SET);
        fwrite(&supplier, sizeof supplier, 1, supplier_db);
    }
}

static bool supplier_db_exists(FILE *supplier_db) {
    long pos;

    pos = 0l;

    if (supplier_db) {
        fseek(supplier_db, 0l, SEEK_END);
        pos = ftell(supplier_db);
    }

    if (pos == 0l) {
        if (supplier_db != NULL) {
            fclose(supplier_db);
        }

        log_info(true, "La base de données fournisseur est vide");
    }

    return (pos != 0l);
}
