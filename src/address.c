#include "address.h"
#include "input.h"

#include <stdio.h>
#include <string.h>

void address_read(struct address *address) {
    char *tmp;
    size_t len;
    bool valid;

    puts("Saisir adresse :");

    /* Saisie du numéro */
    fputs("Numéro : ", stdout);

    do {
        input_read_stdin(&tmp, &len);
        valid = input_validate_street_number(tmp, len);
    } while (!valid);

    strncpy(address->street_num, tmp, sizeof address->street_num);

    /* Saisie du nom */
    fputs("Nom : ", stdout);

    do {
        input_read_stdin(&tmp, &len);
        valid = input_validate_name(tmp, len);
    } while (!valid);

    strncpy(address->street_name, tmp, sizeof address->street_name);

    /* Saisie du code postal */
    fputs("Code postal : ", stdout);

    do {
        input_read_stdin(&tmp, &len);
        valid = input_validate_zip_code(tmp, len);
    } while (!valid);

    strncpy(address->zip_code, tmp, sizeof address->zip_code);

    /* Saisie de la ville */
    fputs("Ville : ", stdout);

    do {
        input_read_stdin(&tmp, &len);
        valid = input_validate_name(tmp, len);
    } while (false);

    strncpy(address->city, tmp, sizeof address->city);

    free(tmp);
}

void address_inspect(const struct address *address) {
    printf(
        "Adresse : %s Rue %s %s %s\n",
        address->street_num,
        address->street_name,
        address->zip_code,
        address->city
    );
}
