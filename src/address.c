#include "address.h"
#include "input.h"
#include "terminal.h"

#include <stdio.h>

void address_read(struct address *address) {
    new_page();
    puts("Informations adresse");

    /* Saisie du numéro */
    input_read_positive_int(
        "Numéro : ",
        "Le numéro de rue doit être positif",
        (int *)&address->num
    );

    /* Saisie du nom */
    input_read_alpha("Nom : ", address->name, sizeof address->name);

    /* Saisie du code postal */
    input_read_num(
        "Code postal : ",
        "Le code postal doit contenir exactement 5 chiffres",
        address->zip_code,
        sizeof address->zip_code,
        5
    );

    /* Saisie de la ville */
    input_read_alpha("Ville : ", address->city, sizeof address->city);
}

void address_inspect(const struct address *address) {
    printf(
        "Adresse : %hu Rue %s %s %s\n",
        address->num,
        address->name,
        address->zip_code,
        address->city
    );
}
