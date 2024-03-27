#include "database.h"
#include "produit.h"
#include "client.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define COUNTOF(x) (sizeof (x) / sizeof *(x))
#define CSI "\x1b["

enum modes {
    MODE_CLIENT,
    MODE_FOURNISSEUR,
    MODE_PRODUIT,
    MODE_VENTE,
    MODE_QUITTER
};

enum actions {
    ACTION_CREER = 0,
    ACTION_MODIFIER,
    ACTION_CONSULTER,
    ACTION_SUPPRIMER,
    ACTION_RETOUR,
    ACTION_QUITTER,
    ACTION_CLIENT_CONSULTER_HISTO_ACHAT = 6,
    ACTION_FOURNISSEUR_PASSER_COMMANDE = 6,
    ACTION_FOURNISSEUR_ENREGISTRER_LIVRAISON,
    ACTION_PRODUIT_CONSULTER_INVENTAIRE = 6,
    ACTION_VENTE_ENREGISTRER_TRANSAC = 0,
    ACTION_VENTE_CONSULTER_HISTO_VENTES
};

enum screen_buffer {
    screen_buffer_default,
    screen_buffer_alternative
};

void clear_screen(void) {
    fputs(CSI "2J", stdout);
}

void use_screen_buffer(enum screen_buffer sb) {
    switch (sb) {
    case screen_buffer_alternative:
        fputs(CSI "?1049h", stdout);
        break;
    case screen_buffer_default:
        fputs(CSI "?1049l", stdout);
        break;
    }
}

void set_cursor_pos(short x, short y) {
    printf(CSI "%hd;%hdH", ++y, ++x);
}

void move_cursor_to_col(short x) {
    printf(CSI "%hdG", x);
}

void set_cursor_home(void) {
    fputs(CSI "H", stdout);
}

void save_cursor_pos(void) {
    fputs(CSI "s", stdout);
}

void restore_cursor_pos(void) {
    fputs(CSI "u", stdout);
}

void show_cursor(bool show) {
    if (show) {
        fputs(CSI "?25h", stdout);
    } else {
        fputs(CSI "?25l", stdout);
    }
}

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
        valid = validate_input(choice, MODE_CLIENT, MODE_QUITTER);

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
                ACTION_CREER,
                ACTION_CLIENT_CONSULTER_HISTO_ACHAT
            );
            break;
        case MODE_FOURNISSEUR:
            valid = validate_input(
                choice,
                ACTION_CREER,
                ACTION_FOURNISSEUR_ENREGISTRER_LIVRAISON
            );
            break;
        case MODE_PRODUIT:
            valid = validate_input(
                choice,
                ACTION_CREER,
                ACTION_PRODUIT_CONSULTER_INVENTAIRE
            );
            break;
        case MODE_VENTE:
            valid = validate_input(
                choice,
                ACTION_VENTE_ENREGISTRER_TRANSAC,
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

    char *action_fournisseur_str[2] = {
        "Passer commande",
        "Enregistrer livraison",
    };

    char *action_produit_str[1] = {
        "Consulter inventaire",
    };

    char *action_vente_str[2] = {
        "Enregistrer transaction",
        "Consulter historique",
    };

    bool quit;

    database_open();
    use_screen_buffer(screen_buffer_alternative);
    show_cursor(false);

    quit = false;

    while (!quit) {
        clear_screen();
        set_cursor_home();

        puts("Choisir mode d'action:");
        list_print(mode_str, COUNTOF(mode_str), 1);

        process_input(&mode);

        if (mode == MODE_QUITTER) {
            quit = true;
            continue;
        }

        clear_screen();
        set_cursor_home();

        printf("Mode %s\n", mode_str[mode]);

        if (mode != MODE_VENTE) {
            list_print(action_base_str, COUNTOF(action_base_str), 1);
        }

        switch (mode) {
        case MODE_CLIENT:
            list_print(action_client_str, COUNTOF(action_client_str), 7);
            break;
        case MODE_FOURNISSEUR:
            list_print(action_fournisseur_str, COUNTOF(action_fournisseur_str), 7);
            break;
        case MODE_PRODUIT:
            list_print(action_produit_str, COUNTOF(action_produit_str), 7);
            break;
        case MODE_VENTE:
            list_print(action_vente_str, COUNTOF(action_vente_str), 1);
            break;
        default:
            break;
        }

        mode_process_input(mode, &action);

        if (action == ACTION_QUITTER) {
            quit = true;
            continue;
        }

        switch (mode) {
        case MODE_CLIENT:
            switch (action) {
            case ACTION_CREER:
                register_client();
                break;
            case ACTION_MODIFIER:
                puts("pas encore implémenté");
                break;
            case ACTION_CONSULTER:
                puts("pas encore implémenté");
                break;
            case ACTION_SUPPRIMER:
                puts("pas encore implémenté");
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
            case ACTION_CREER:
                puts("pas encore implémenté");
                break;
            case ACTION_MODIFIER:
                puts("pas encore implémenté");
                break;
            case ACTION_CONSULTER:
                puts("pas encore implémenté");
                break;
            case ACTION_SUPPRIMER:
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
            case ACTION_CREER:
                register_product();
                break;
            case ACTION_MODIFIER:
                puts("pas encore implémenté");
                break;
            case ACTION_CONSULTER:
                puts("pas encore implémenté");
                break;
            case ACTION_SUPPRIMER:
                puts("pas encore implémenté");
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
        }

        if (action != ACTION_RETOUR) {
            getchar();
        }

        puts("");
    }

    database_close();

    show_cursor(true);
    use_screen_buffer(screen_buffer_default);

    return 0;
}
