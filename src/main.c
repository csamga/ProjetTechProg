#include "database.h"
#include "produit.h"
#include "client.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define COUNTOF(x) (sizeof (x) / sizeof *(x))

enum modes {
    MODE_CLIENT,
    MODE_FOURNISSEUR,
    MODE_PRODUIT,
    MODE_VENTE,
    MODE_QUITTER
};

enum actions_base {
    ACTION_BASE_ENREGISTRER,
    ACTION_BASE_MODIFIER,
    ACTION_BASE_CONSULTER,
    ACTION_BASE_SUPPRIMER
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

void process_input(enum modes *mode) {
    int choice;
    bool valid;

    do {
        choice = getchar();
        flush_stdin();

        choice -= '1';

        valid = (choice >= MODE_CLIENT) && (choice <= MODE_QUITTER);

        if (!valid) {
            puts("Veuillez choisir un mode existant");
        }
    } while (!valid);

    *mode = (enum modes)choice;
}

/* void process_input(bool *quit) {

    char choice, c;
    enum db_type db_type;
    char name[50];
    short id;
    struct client client;
    struct product product;
    bool exists;

    choice = getchar();
    while (((c = getchar()) != '\n') && (c != EOF)) {}
    puts("");
    
    switch (choice) {
    case '1':
        register_client();
        break;
    case '2':
        register_product();
        break;
    case '3':
    case '4':
        switch (choice) {
        case '3':
            db_type = CLIENT;
            break;
        case '4':
            db_type = PRODUCT;
            break;
        }

        puts(
            "Recherche par :\n"
            "1: Identifiant\n"
            "2: Nom"
        );

        choice = getchar();
        while (((c = getchar()) != '\n') && (c != EOF)) {}
        puts("");

        switch (choice) {
        case '1':
            if (db_type == CLIENT) {
                fputs("Identifiant client : ", stdout);
                scanf("%hd", &id);

                search_client_by_id(id, &client, &exists);

                if (exists) {
                    print_client(&client);
                }
            } else {
                fputs("Nom produit : ", stdout);
                fgets(name, sizeof name, stdin);
                name[strcspn(name, "\n")] = '\0';

                search_product_by_name(name, &product, &exists);

                if (exists) {
                    print_product(&product);
                }
            }
            break;
        case '2':
            if (db_type == CLIENT) {
                fputs("Nom client : ", stdout);
                fgets(name, sizeof name, stdin);
                name[strcspn(name, "\n")] = '\0';

                search_client_by_name(name, &client, &exists);

                if (exists) {
                    print_client(&client);
                }
            } else {
                fputs("Nom produit : ", stdout);
                fgets(name, sizeof name, stdin);
                name[strcspn(name, "\n")] = '\0';

                search_product_by_name(name, &product, &exists);

                if (exists) {
                    print_product(&product);
                }
            }
            break;
        }

        break;
    case '5':
        *quit = true;
        break;
    }
} */

int main(void) {
    enum modes mode;

    char *mode_str[5] = {
        "Client",
        "Fournisseur",
        "Produit",
        "Vente",
        "Quitter",
    };

    char *action_base_str[4] = {
        "Créer",
        "Modifier",
        "Consulter",
        "Supprimer"
    };

    char *action_client_str[1] = {
        "Consulter historique achat"
    };

    char *action_fournisseur_str[2] = {
        "Passer commande",
        "Enregistrer livraison"
    };

    char *action_produit_str[1] = {
        "Consulter inventaire"
    };

    char *action_vente_str[2] = {
        "Enregistrer transaction",
        "Consulter historique"
    };

    bool quit;

    database_open();

    quit = false;

    while (!quit) {
        puts("Choisir mode d'action:");
        list_print(mode_str, COUNTOF(mode_str), 1);

        process_input(&mode);

        if (mode != MODE_QUITTER) {
            printf("Mode %s\n", mode_str[mode]);
            list_print(action_base_str, COUNTOF(action_base_str), 1);
        }

        switch (mode) {
        case MODE_CLIENT:
            list_print(action_client_str, COUNTOF(action_client_str), 5);
            break;
        case MODE_FOURNISSEUR:
            list_print(action_fournisseur_str, COUNTOF(action_fournisseur_str), 5);
            break;
        case MODE_PRODUIT:
            list_print(action_produit_str, COUNTOF(action_produit_str), 5);
            break;
        case MODE_VENTE:
            list_print(action_vente_str, COUNTOF(action_vente_str), 5);
            break;
        case MODE_QUITTER:
            quit = true;
            break;
        }

        puts("");
    }

    database_close();

    return 0;
}
