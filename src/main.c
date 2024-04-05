#include "base.h"
#include "terminal.h"
#include "input.h"
#include "utils.h"
#include "product.h"
#include "sale.h"
#include "client.h"

#include <stdbool.h>
#include <stdio.h>
#include <wchar.h>
#include <locale.h>

enum actions {
    ACTION_NONE = 0,
    ACTION_REGISTER = 0,
    ACTION_MODIFY,
    ACTION_INSPECT,
    ACTION_DELETE,
    ACTION_RETURN,
    ACTION_QUIT,
    ACTION_CLIENT_CONSULTER_HISTO_ACHAT = 6,
    ACTION_FOURNISSEUR_PASSER_COMMANDE = 6,
    ACTION_FOURNISSEUR_ENREGISTRER_LIVRAISON,
    ACTION_PRODUIT_CONSULTER_INVENTAIRE = 6,
    ACTION_VENTE_ENREGISTRER_TRANSAC = 6,
    ACTION_VENTE_CONSULTER_HISTO_VENTES
};

enum search_mode {
    SEARCH_ID,
    SEARCH_NAME
};

void process_input(enum modes *mode) {
    int choice;
    bool valid;

    do {
        choice = acquire_input();
        valid = validate_input(choice, MODE_CLIENT, MODE_QUIT);

        if (!valid) {
            puts("Veuillez choisir un mode existant");
        }
    } while (!valid);

    *mode = (enum modes)choice;
}

void mode_process_input(enum modes mode, enum actions *action) {
    int choice;
    bool valid;

    do {
        choice = acquire_input();

        switch (mode) {
        case MODE_CLIENT:
            valid = validate_input(
                choice,
                ACTION_REGISTER,
                ACTION_CLIENT_CONSULTER_HISTO_ACHAT
            );
            break;
        case MODE_SUPPLIER:
            valid = validate_input(
                choice,
                ACTION_REGISTER,
                ACTION_FOURNISSEUR_ENREGISTRER_LIVRAISON
            );
            break;
        case MODE_PRODUCT:
            valid = validate_input(
                choice,
                ACTION_REGISTER,
                ACTION_PRODUIT_CONSULTER_INVENTAIRE
            );
            break;
        case MODE_SALE:
            valid = validate_input(
                choice + 4,
                ACTION_RETURN,
                ACTION_VENTE_CONSULTER_HISTO_VENTES
            );
            *action = (enum actions)choice + 4;
            return;
        default:
            valid = true;
            break;
        }

        if (!valid) {
            puts("Veuillez choisir une action existante");
        }
    } while (!valid);

    *action = (enum actions)choice;
}

/* void search(enum modes mode) {
    int choice;
    bool valid;

    puts(
        "Recherche par :\n"
        "1: Identifiant\n"
        "2: Nom"
    );

    do {
        choice = acquire_input();
        valid = validate_input(choice, SEARCH_NAME, SEARCH_ID);

        if (!valid) {
            puts("-_- MOAI -_-");
        }
    } while (!valid);

    switch (choice) {
    case SEARCH_NAME:
        if (db_type == CLIENT) {
            fputs("Identifiant client : ", stdout);
            scanf("%hd", &id);

            search_client_by_id(id, &client, &exists);
        } else {
            fputs("Nom produit : ", stdout);
            fgets(name, sizeof name, stdin);
            name[strcspn(name, "\n")] = '\0';

            search_product_by_name(name, &product, &exists);

            if (exists) {
                print_product(&product);
            }
        }
    }
} */

int main(void) {
    enum modes mode;
    enum actions action;

    char *mode_str[5] = {
        "Client",
        "Fournisseur",
        "Produit",
        "Vente",
        "Quitter",
    };

    char *action_base_str[6] = {
        "Creer",
        "Modifier",
        "Consulter",
        "Supprimer",
        "Retour",
        "Quitter"
    };

    char *action_client_str[1] = {
        "Consulter historique achat",
    };

    char *action_supplier_str[2] = {
        "Passer commande",
        "Enregistrer livraison",
    };

    char *action_product_str[1] = {
        "Consulter inventaire",
    };

    char *action_sale_str[2] = {
        "Enregistrer transaction",
        "Consulter historique",
    };

    /* char *action_rech_str[2] = {
        "Recherche par NOM",
        "Recherche par ID"
    }; */

    bool quit;

    use_screen_buffer(screen_buffer_alternative);
    show_cursor(false);

    quit = false;

    while (!quit) {
        new_page();

        puts("Choisir mode d'action:");
        list_print(mode_str, COUNTOF(mode_str), 1);

        process_input(&mode);

        quit = (mode == MODE_QUIT);
        action = ACTION_NONE;

        while ((action != ACTION_RETURN) && !quit) {
            new_page();

            printf("Mode %s\n", mode_str[mode]);

            if (mode != MODE_SALE) {
                list_print(action_base_str, COUNTOF(action_base_str), 1);
            } else {
                list_print(&action_base_str[ACTION_RETURN], 2, 1);
            }

            switch (mode) {
            case MODE_CLIENT:
                list_print(action_client_str, COUNTOF(action_client_str), 7);
                break;
            case MODE_SUPPLIER:
                list_print(action_supplier_str, COUNTOF(action_supplier_str), 7);
                break;
            case MODE_PRODUCT:
                list_print(action_product_str, COUNTOF(action_product_str), 7);
                break;
            case MODE_SALE:
                list_print(action_sale_str, COUNTOF(action_sale_str), 3);
                break;
            default:
                break;
            }

            mode_process_input(mode, &action);

            switch (action) {
            case ACTION_REGISTER:
                base_register(mode);
                break;
            case ACTION_MODIFY:
                base_modify(mode);
                break;
            case ACTION_INSPECT:
                base_inspect(mode);
                break;
            case ACTION_DELETE:
                base_delete(mode);
                break;
            case ACTION_RETURN:
                break;
            case ACTION_QUIT:
                quit = true;
                break;
            default:
                break;
                /* puts("implémenter");
                getchar();
                break; */
            }

            switch (mode) {
            case MODE_CLIENT:
                switch (action) {
                case ACTION_CLIENT_CONSULTER_HISTO_ACHAT:
                    client_print_history();
                    break;
                default:
                    break;
                }
                break;
            case MODE_SUPPLIER:
                break;
            case MODE_SALE:
                switch (action) {
                case ACTION_VENTE_ENREGISTRER_TRANSAC:
                    sale_register();
                    break;
                case ACTION_VENTE_CONSULTER_HISTO_VENTES:
                    puts("implémenter histo ventes");
                    getchar();
                    break;
                default:
                    break;
                }
                break;
            case MODE_PRODUCT:
                switch (action) {
                case ACTION_PRODUIT_CONSULTER_INVENTAIRE:
                    product_inspect_inventory();
                    break;
                default:
                    break;
                }
                break;
            }
        }
    }

    show_cursor(true);
    use_screen_buffer(screen_buffer_default);

    return 0;
}
