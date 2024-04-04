#include "input.h"
#include "terminal.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define CHUNK 256

static void input_read_stdin(char **input, size_t *len);
static bool input_validate_alpha(char *input, size_t len);
static bool input_validate_num(char *reject_prompt, char *input, size_t len, size_t exact_len);
static bool input_validate_email(char *input, size_t len);

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

void input_read_alpha(char *prompt, char *dest, size_t max_len) {
    bool valid;
    char *tmp;
    size_t len;

    fputs(prompt, stdout);

    do {
        input_read_stdin(&tmp, &len);
        valid = input_validate_alpha(tmp, len);
    } while (!valid);

    strncpy(dest, tmp, max_len);

    free(tmp);
}

void input_read_num(char *prompt, char *reject_prompt, char *dest, size_t max_len, size_t exact_len) {
    bool valid;
    char *tmp;
    size_t len;

    fputs(prompt, stdout);

    do {
        input_read_stdin(&tmp, &len);
        valid = input_validate_num(reject_prompt, tmp, strlen(tmp), exact_len);
    } while (!valid);

    strncpy(dest, tmp, max_len);

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

void input_read_positive_float(char *prompt, char *reject_prompt, float *dest) {
    bool valid;
    float tmp;

    fputs(prompt, stdout);
    
    do {
        scanf("%f", &tmp);
        getchar();
        valid = (tmp >= 0.0f);
        if (!valid) {
            puts(reject_prompt);
        }
    } while (!valid);

    *dest = tmp;
}

void input_read_positive_int(char *prompt, char *reject_prompt, int *dest) {
    bool valid;
    int tmp;

    fputs(prompt, stdout);

    do {
        scanf("%d", &tmp);
        getchar();
        valid = (tmp >= 0);
        if (!valid) {
            puts(reject_prompt);
        }
    } while (!valid);

    *dest = tmp;
}

bool input_confirm_delete(const char *prompt) {
    char c;
    bool valid, confirm;

    new_page();

    do {
        printf("%s (o/N) ", prompt);
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

static void input_read_stdin(char **input, size_t *len) {
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

static bool input_validate_alpha(char *input, size_t len) {
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

static bool input_validate_num(char *prompt, char *input, size_t len, size_t exact_len) {
    bool valid;
    size_t i;

    valid = (len == exact_len);

    for (i = 0; i < len && valid; i++) {
        valid = isdigit(input[i]);
    }

    if (!valid) {
        puts(prompt);
    }

    return valid;
}

static bool input_validate_email(char *input, size_t len) {
    bool valid;

    (void)input;
    (void)len;

    valid = true;

    return valid;
}

/* static bool input_validate_street_number(char *input, size_t len) {
    bool valid;

    (void)input;
    (void)len;

    valid = true;

    return valid;
} */

/* static bool input_validate_zip_code(char *input, size_t len) {
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
} */
