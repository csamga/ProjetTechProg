#include "database.h"
#include "produit.h"
#include "client.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

void process_input(bool *quit) {
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
}

int main(void) {
    bool quit;

    database_open();

    quit = false;

    while (!quit) {
        puts("Actions :");
        puts("1: Enregistrer client");
        puts("2: Enregistrer produit");
        puts("3: Afficher informations client");
        puts("4: Afficher informations produit");
        puts("5: Quitter");
        process_input(&quit);
    }

    database_close();

    return 0;
}
