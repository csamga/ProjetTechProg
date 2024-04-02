#include "input.h"
#include "terminal.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define CHUNK 256

int acquire_input(void) {
    int choice;

    choice = getchar();
    choice -= '1';

    input_flush_stdin();

    return choice;
}

bool validate_input(int choice, int first, int last) {
    return (choice >= first) && (choice <= last);
}

void input_read_stdin(char **input, size_t *len) {
    char tmp_buf[CHUNK];
    size_t tmp_buf_len;

    *input = NULL;
    *len = 0;

    do {
        fgets(tmp_buf, CHUNK, stdin);

        tmp_buf[strcspn(tmp_buf, "\n")] = '\0';
        tmp_buf_len = strlen(tmp_buf);

        *input = realloc(*input, *len + tmp_buf_len + 1);
        strcpy(*input + *len, tmp_buf);
        *len += tmp_buf_len;
    } while (tmp_buf_len == CHUNK - 1 && tmp_buf[CHUNK - 2] != '\n');
}

void input_read_last_name(char *name, size_t max_len) {
    bool valid;
    char *tmp;
    size_t len;

    fputs("Nom : ", stdout);

    do {
        input_read_stdin(&tmp, &len);
        valid = input_validate_name(tmp, len);
    } while (!valid);

    strncpy(name, tmp, max_len);

    free(tmp);
}

void input_read_first_name(char *name, size_t max_len) {
    bool valid;
    char *tmp;
    size_t len;

    fputs("Prénom : ", stdout);

    do {
        input_read_stdin(&tmp, &len);
        valid = input_validate_name(tmp, len);
    } while (!valid);

    strncpy(name, tmp, max_len);

    free(tmp);
}

void input_read_phone(char *phone, size_t max_len) {
    bool valid;
    char *tmp;
    size_t len;

    fputs("Tel : ", stdout);

    do {
        input_read_stdin(&tmp, &len);
        valid = input_validate_phone(tmp, len);
    } while (!valid);

    strncpy(phone, tmp, max_len);

    free(tmp);
}

void input_read_email(char *email, size_t max_len) {
    bool valid;
    char *tmp;
    size_t len;

    fputs("Email : ", stdout);

    do {
        input_read_stdin(&tmp, &len);
        valid = input_validate_email(tmp, len);
    } while (!valid);

    strncpy(email, tmp, max_len);

    free(tmp);
}

void input_read_price(float *price) {
    bool valid;
    float tmp;

    fputs("Prix : ", stdout);
    
    do {
        scanf("%f", &tmp);
        valid = input_validate_price(tmp);
    } while (!valid);

    *price = tmp;
}

bool input_validate_name(char *input, size_t len) {
    bool valid;
    size_t i;
    char c;

    valid = ((len > 0) && (len <= 30));

    for (i = 0; i < len && valid; i++) {
        c = input[i];
        valid = (isalpha(c) || (c == ' ') || (c == '-'));
    }

    if (!valid) {
        puts(
            "Le nom ne peut contenir que des lettres, espaces et tirets "
            "et doit faire entre 1 et 30 caractères"
        );
    }

    return valid;
}

bool input_validate_phone(char *input, size_t len) {
    bool valid;
    size_t i;

    valid = (len == 10);

    for (i = 0; i < len && valid; i++) {
        valid = isdigit(input[i]);
    }

    if (!valid) {
        puts("Le numéro de téléphone doit comporter 10 chiffres");
    }

    return valid;
}

bool input_validate_email(char *input, size_t len) {
    bool valid;

    (void)input;
    (void)len;

    valid = true;

    return valid;
}

bool input_validate_street_number(char *input, size_t len) {
    bool valid;

    (void)input;
    (void)len;

    valid = true;

    return valid;
}

bool input_validate_zip_code(char *input, size_t len) {
    bool valid;
    size_t i;

    valid = (len == 5);

    for (i = 0; i < len && valid; i++) {
        valid = isdigit(input[i]);
    }

    if (!valid) {
        puts("Le code postal doit comporter 5 chiffres");
    }

    return valid;
}

bool input_validate_price(float price) {
    return (price >= 0.0f);
}

bool input_confirm_delete(const char *message) {
    char c;
    bool valid, confirm;

    new_page();

    do {
        printf("%s (o/N)", message);
        scanf("%c", &c);

        switch (c) {
        case '\n':
            valid = true;
            confirm = true;
            continue;
        case 'o':
        case 'O':
            valid = true;
            confirm = true;
            break;
        case 'n':
        case 'N':
            valid = true;
            confirm = false;
            break;
        default:
            valid = false;
        }

        input_flush_stdin();
    } while (!valid);

    return confirm;
}

void input_flush_stdin(void) {
    int c;

    while (((c = getchar()) != '\n') && (c != EOF)) {}
}

