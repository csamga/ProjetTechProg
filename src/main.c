#include "base.h"
#include "terminal.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define COUNTOF(x) (sizeof (x) / sizeof *(x))

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

void list_print(char *list[], short n, int base) {
#define CHUNK 256
    short i;
    char tmp[CHUNK];
    char *buf;
    int buf_len, written, tmp_written;

    buf = malloc(CHUNK * (sizeof *buf));
    buf_len = CHUNK;
    written = 0;

    for (i = 0; i < n; i++) {
        tmp_written = sprintf(tmp, "%hd: %s\n", base + i, list[i]);

        if (written + tmp_written >= buf_len) {
            buf = realloc(buf, (buf_len * 2) * (sizeof *buf));
        }

        strcpy(buf + written, tmp);
        written += tmp_written;
    }

    fputs(buf, stdout);
    free(buf);
}

void flush_stdin(void) {
    int c;

    while (((c = getchar()) != '\n') && (c != EOF)) {}
}

int acquire_input(void) {
    int choice;

    choice = getchar();
    choice -= '1';

    flush_stdin();

    return choice;
}

bool validate_input(int choice, int first, int last) {
    return (choice >= first) && (choice <= last);
}

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
                choice,
                ACTION_RETURN,
                ACTION_VENTE_CONSULTER_HISTO_VENTES
            );
            break;
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

    char *action_rech_str[2] = {
        "Recherche par NOM",
        "Recherche par ID"
    };

    bool quit;

    /* database_open(); */
    use_screen_buffer(screen_buffer_alternative);
    show_cursor(false);

    quit = false;

    while (!quit) {
        clear_screen();
        set_cursor_home();

        puts("Choisir mode d'action:");
        list_print(mode_str, COUNTOF(mode_str), 1);

        process_input(&mode);

        quit = (mode == MODE_QUIT);
        action = ACTION_NONE;

        while ((action != ACTION_RETURN) && !quit) {
            clear_screen();
            set_cursor_home();

            printf("Mode %s\n", mode_str[mode]);

            if (mode != MODE_SALE) {
                list_print(action_base_str, COUNTOF(action_base_str), 1);
            } else {
                puts(action_base_str[ACTION_RETURN]);
                puts(action_base_str[ACTION_QUIT]);
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
                list_print(action_sale_str, COUNTOF(action_sale_str), 1);
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
                puts("implémenter base modify");
                getchar();
                break;
            case ACTION_INSPECT:
                base_inspect(mode);
                break;
            case ACTION_DELETE:
                puts("implémenter base delete");
                getchar();
                break;
            case ACTION_RETURN:
                break;
            case ACTION_QUIT:
                quit = true;
                break;
            default:
                puts("implémenter");
                getchar();
                break;
            }
        }

        /* switch (mode) {
        case MODE_CLIENT:
            switch (action) {
            case ACTION_REGISTER:
                client_register();
                break;
            case ACTION_MODIFY:
                client_modify();
                break;
            case ACTION_INSPECT:
                client_print();
                break;
            case ACTION_DELETE:
                client_delete();
                break;
            case ACTION_CLIENT_CONSULTER_HISTO_ACHAT:
                puts("pas encore implémenté");
                break;
            default:
                break;
            }
            break;
        case MODE_FOURNISSEUR:
            switch (action) {
            case ACTION_REGISTER:
                fournisseur_register();
                break;
            case ACTION_MODIFY:
                puts("pas encore implémenté");
                break;
            case ACTION_INSPECT:
                puts("pas encore implémenté");
                break;
            case ACTION_DELETE:
                puts("pas encore implémenté");
                break;
            case ACTION_FOURNISSEUR_PASSER_COMMANDE:
                puts("pas encore implémenté");
                break;
            case ACTION_FOURNISSEUR_ENREGISTRER_LIVRAISON:
                puts("pas encore implémenté");
                break;
            default:
                break;
            }
            break;
        case MODE_PRODUIT:
            switch (action) {
            case ACTION_REGISTER:
                product_register();
                break;
            case ACTION_MODIFY:
                product_modify();
                break;
            case ACTION_INSPECT:
                product_print();
                break;
            case ACTION_DELETE:
                product_delete();
                break;
            case ACTION_PRODUIT_CONSULTER_INVENTAIRE:
                puts("pas encore implémenté");
                break;
            default:
                break;
            }
            break;
        case MODE_VENTE:
            switch (action) {
            case ACTION_VENTE_ENREGISTRER_TRANSAC:
                puts("pas encore implémenté");
                break;
            case ACTION_VENTE_CONSULTER_HISTO_VENTES:
                puts("pas encore implémenté");
                break;
            default:
                break;
            }
            break;
        default:
            break;
        } */
    }

    /* database_close(); */

    show_cursor(true);
    use_screen_buffer(screen_buffer_default);

    return 0;
}
