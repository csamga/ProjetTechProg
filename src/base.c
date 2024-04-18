#include "base.h"

#include "client.h"
#include "input.h"
#include "market.h"
#include "product.h"
#include "sale.h"
#include "supplier.h"
#include "terminal.h"
#include "utils.h"

enum mode {
    mode_client = 1,
    mode_supplier,
    mode_product,
    mode_sale,
    mode_quit
};

enum action {
    action_none = 0,
    action_register,
    action_modify,
    action_inspect,
    action_delete,
    action_return,
    action_quit,
    action_client_consulter_histo_achat = (action_quit + 1),
    action_sale_register = (action_quit + 1),
    action_supplier_place_order = (action_quit + 1),
    action_supplier_inspect_order,
    action_supplier_register_delivery,
    action_product_consulter_inventaire = (action_quit + 1)
};

static char *mode_str[] = {
    "Client",
    "Fournisseur",
    "Produit",
    "Vente",
    "Quitter",
};

static char *action_base_str[] = {
    "Creer",
    "Modifier",
    "Consulter",
    "Supprimer",
    "Retour",
    "Quitter"
};

static char *action_client_str[] = {
    "Consulter historique achat",
};

static char *action_supplier_str[] = {
    "Passer commande",
    "Consulter commande",
    "Enregistrer livraison",
};

static char *action_product_str[] = {
    "Consulter inventaire",
};

static char *action_sale_str[] = {
    "Enregistrer transaction",
};

static enum mode choose_mode(void);
static enum action choose_action(enum mode mode);

static bool is_base_action(enum action action);
static void call_base_action(enum action action, enum mode mode, bool *quit);
static void call_mode_action(enum action action, enum mode mode);

static enum mode process_mode_input(void);
static enum action process_action_input(enum mode mode);

static void base_register(enum mode mode);
static void base_inspect(enum mode mode);
static void base_modify(enum mode mode);
static void base_delete(enum mode mode);

void init(void) {
    use_screen_buffer(screen_buffer_alternative, stdout);
    use_screen_buffer(screen_buffer_alternative, stderr);
    show_cursor(false);

    market_create_db();
}

void run(void) {
    enum mode mode;
    enum action action;
    bool quit;

    quit = false;

    while (!quit) {
        mode = choose_mode();

        quit = (mode == mode_quit);
        action = action_none;

        while (!quit && (action != action_return)) {
            action = choose_action(mode);

            new_page();

            if (is_base_action(action)) {
                call_base_action(action, mode, &quit);
            } else {
                call_mode_action(action, mode);
            }
        }
    }
}

void finish(void) {
    show_cursor(true);
    use_screen_buffer(screen_buffer_default, stdout);
    use_screen_buffer(screen_buffer_default, stderr);
}

static enum mode choose_mode(void) {
    new_page();

    log_title("Choix mode d'action");
    list_print(mode_str, COUNTOF(mode_str), 1);

    return process_mode_input();
}

static enum action choose_action(enum mode mode) {
    new_page();

    log_title("Mode %s", mode_str[mode - 1]);

    if (mode != mode_sale) {
        list_print(action_base_str, COUNTOF(action_base_str), 1);
    } else {
        list_print(
            &action_base_str[action_return - 1],
            action_quit - action_delete,
            1
        );
    }

    switch (mode) {
    case mode_client:
        list_print(
            action_client_str,
            COUNTOF(action_client_str),
            action_quit + 1
        );
        break;
    case mode_supplier:
        list_print(
            action_supplier_str,
            COUNTOF(action_supplier_str),
            action_quit + 1
        );
        break;
    case mode_product:
        list_print(
            action_product_str,
            COUNTOF(action_product_str),
            action_quit + 1
        );
        break;
    case mode_sale:
        list_print(
            action_sale_str,
            COUNTOF(action_sale_str),
            action_quit - action_delete + 1
        );
        break;
    default:
        break;
    }

    return process_action_input(mode);
}

static bool is_base_action(enum action action) {
    return (action >= action_register) && (action <= action_quit);
}

static void call_base_action(enum action action, enum mode mode, bool *quit) {
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
        *quit = true;
        break;
    default:
        break;
    }
}

static void call_mode_action(enum action action, enum mode mode) {
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

static enum mode process_mode_input(void) {
    int choice;
    bool valid;

    do {
        choice = menu_acquire_input() + 1;
        valid = menu_validate_input(choice, mode_client, mode_quit);

        if (!valid) {
            log_warning(true, "Veuillez choisir un mode existant");
            move_cursor_up(1);
            erase_line();
            move_cursor_up(1);
            erase_line();
        }
    } while (!valid);

    return (enum mode)choice;
}

static enum action process_action_input(enum mode mode) {
    int choice;
    bool valid;

    do {
        choice = menu_acquire_input() + 1;

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
            choice += 4;
            valid = menu_validate_input(
                choice,
                action_return,
                action_sale_register
            );
            break;
        default:
            valid = true;
            break;
        }

        if (!valid) {
            log_warning(true, "Veuillez choisir une action existante");
            move_cursor_up(1);
            erase_line();
            move_cursor_up(1);
            erase_line();
        }
    } while (!valid);

    return (enum action)choice;
}

static void base_register(enum mode mode) {
    switch (mode) {
    case mode_client:
        client_register();
        break;
    case mode_supplier:
        supplier_register();
        break;
    case mode_product:
        product_register();
        break;
    default:
        break;
    }
}

static void base_inspect(enum mode mode) {
    switch (mode) {
    case mode_client:
        client_inspect();
        break;
    case mode_supplier:
        supplier_inspect();
        break;
    case mode_product:
        product_inspect();
        break;
    default:
        break;
    }
}

static void base_modify(enum mode mode) {
    switch (mode) {
    case mode_client:
        client_modify();
        break;
    case mode_supplier:
        supplier_modify();
        break;
    case mode_product:
        product_modify();
        break;
    default:
        break;
    }
}

static void base_delete(enum mode mode) {
    switch (mode) {
    case mode_client:
        client_delete();
        break;
    case mode_supplier:
        supplier_delete();
        break;
    case mode_product:
        product_delete();
        break;
    default:
        break;
    }
}
