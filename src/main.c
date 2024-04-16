#include "base.h"
#include "terminal.h"
#include "input.h"
#include "utils.h"
#include "product.h"
#include "sale.h"
#include "client.h"
#include "market.h"
#include "supplier.h"

#include <stdbool.h>
#include <stdio.h>

enum actions {
    action_none = 0,
    action_register = 0,
    action_modify,
    action_inspect,
    action_delete,
    action_return,
    action_quit,
    action_client_consulter_histo_achat = 6,
    action_sale_register = 6,
    action_sale_inspect_history,
    action_supplier_place_order = 6,
    action_supplier_inspect_order,
    action_supplier_register_delivery,
    action_product_consulter_inventaire = 6
};

enum search_mode {
    SEARCH_ID,
    SEARCH_NAME
};

void process_mode_input(enum modes *mode) {
    int choice;
    bool valid;

    do {
        choice = menu_acquire_input();
        valid = menu_validate_input(choice, mode_client, mode_quit);

        if (!valid) {
            set_text_attr(yellow, true);
            fputs("Veuillez choisir un mode existant", stdout);
            reset_text_attr();
            getchar();
            move_cursor_up(1);
            erase_line();
            move_cursor_up(1);
            erase_line();
        }
    } while (!valid);

    *mode = (enum modes)choice;
}

void process_action_input(enum modes mode, enum actions *action) {
    int choice;
    bool valid;

    do {
        choice = menu_acquire_input();

        switch (mode) {
        case mode_client:
            valid = menu_validate_input(
                choice,
                action_register,
                action_client_consulter_histo_achat
            );
            break;
        case mode_supplier:
            valid = menu_validate_input(
                choice,
                action_register,
                action_supplier_register_delivery
            );
            break;
        case mode_product:
            valid = menu_validate_input(
                choice,
                action_register,
                action_product_consulter_inventaire
            );
            break;
        case mode_sale:
            valid = menu_validate_input(
                choice + 4,
                action_return,
                action_sale_inspect_history
            );
            *action = (enum actions)choice + 4;
            return;
        default:
            valid = true;
            break;
        }

        if (!valid) {
            set_text_attr(yellow, true);
            fputs("Veuillez choisir une action existante", stdout);
            reset_text_attr();
            getchar();
            move_cursor_up(1);
            erase_line();
            move_cursor_up(1);
            erase_line();
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

    char *action_supplier_str[3] = {
        "Passer commande",
        "Consulter commande",
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

    market_create_db();

    use_screen_buffer(screen_buffer_alternative);
    show_cursor(false);

    quit = false;

    while (!quit) {
        new_page();

        set_text_attr(normal, true);
        puts("Choisir mode d'action");
        reset_text_attr();
        list_print(mode_str, COUNTOF(mode_str), 1);

        process_mode_input(&mode);

        quit = (mode == mode_quit);
        action = action_none;

        while ((action != action_return) && !quit) {
            new_page();

            set_text_attr(normal, true);
            printf("Mode %s\n", mode_str[mode]);
            reset_text_attr();

            if (mode != mode_sale) {
                list_print(action_base_str, COUNTOF(action_base_str), 1);
            } else {
                list_print(&action_base_str[action_return], 2, 1);
            }

            switch (mode) {
            case mode_client:
                list_print(action_client_str, COUNTOF(action_client_str), 7);
                break;
            case mode_supplier:
                list_print(action_supplier_str, COUNTOF(action_supplier_str), 7);
                break;
            case mode_product:
                list_print(action_product_str, COUNTOF(action_product_str), 7);
                break;
            case mode_sale:
                list_print(action_sale_str, COUNTOF(action_sale_str), 3);
                break;
            default:
                break;
            }

            process_action_input(mode, &action);

            switch (action) {
            case action_register:
                base_register(mode);
                break;
            case action_modify:
                base_modify(mode);
                break;
            case action_inspect:
                base_inspect(mode);
                break;
            case action_delete:
                base_delete(mode);
                break;
            case action_return:
                break;
            case action_quit:
                quit = true;
                break;
            default:
                break;
            }

            if (quit) {
                break;
            }

            switch (mode) {
            case mode_client:
                switch (action) {
                case action_client_consulter_histo_achat:
                    client_print_history();
                    break;
                default:
                    break;
                }
                break;
            case mode_sale:
                switch (action) {
                case action_sale_register:
                    sale_register();
                    break;
                case action_sale_inspect_history:
                    puts("implémenter histo ventes");
                    getchar();
                    break;
                default:
                    break;
                }
                break;
            case mode_supplier:
                switch (action) {
                case action_supplier_place_order:
                    supplier_place_order();
                    break;
                case action_supplier_inspect_order:
                    supplier_inspect_order();
                    break;
                case action_supplier_register_delivery:
                    supplier_register_delivery();
                    break;
                default:
                    break;
                }
                break;
            case mode_product:
                switch (action) {
                case action_product_consulter_inventaire:
                    product_inspect_inventory();
                    break;
                default:
                    break;
                }
                break;
            default:
                break;
            }
        }
    }

    show_cursor(true);
    use_screen_buffer(screen_buffer_default);

    return 0;
}
