#include "input.h"
#include "terminal.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define CHUNK 256

static void input_read_stdin(char **input, size_t *len);
static bool input_validate_alpha(char *input, size_t len);
static bool input_validate_alphanum(char *input, size_t len);
static bool input_validate_num(char *reject_prompt, char *input, size_t len, size_t exact_len);
static bool input_validate_email(char *reject_prompr, char *input, size_t len);

int menu_acquire_input(void) {
    int choice;

    /* lire un caractère et centrer la valeur en 0 */
    choice = getchar() - '1';
    input_flush_stdin();

    return choice;
}

bool menu_validate_input(int choice, int first, int last) {
    /* vérifier l'encadrement du choix par deux valeurs */
    return (choice >= first) && (choice <= last);
}

void input_read_alpha(char *prompt, char *dest, size_t max_len) {
    bool valid;
    char *tmp;
    size_t len;

    do {
        fputs(prompt, stdout);
        input_read_stdin(&tmp, &len);
        /* vérifie si la chaîne saisie est alphanumérique */
        valid = input_validate_alpha(tmp, len);
    } while (!valid);

    strncpy(dest, tmp, max_len);

    free(tmp);
}

void input_read_num(char *prompt, char *reject_prompt, char *dest, size_t max_len, size_t exact_len) {
    bool valid;
    char *tmp;
    size_t len;

    do {
        fputs(prompt, stdout);
        input_read_stdin(&tmp, &len);
        /* vérifie si le numéro saisi est numérique et comporte 10 chiffres */
        valid = input_validate_num(reject_prompt, tmp, strlen(tmp), exact_len);
    } while (!valid);

    strncpy(dest, tmp, max_len);

    free(tmp);
}

void input_read_alphanum(char *prompt, char *dest, size_t max_len) {
    bool valid;
    char *tmp;
    size_t len;

    do {
        fputs(prompt, stdout);
        input_read_stdin(&tmp, &len);
        valid = input_validate_alphanum(tmp, len);
    } while (!valid);

    strncpy(dest, tmp, max_len);

    free(tmp);
}

void input_read_email(char *prompt, char *reject_prompt, char *email, size_t max_len) {
    bool valid;
    char *tmp;
    size_t len;

    do {
        fputs(prompt, stdout);
        input_read_stdin(&tmp, &len);
        /* la vérification de l'adresse email n'est pas implémentée */
        valid = input_validate_email(reject_prompt, tmp, len);
    } while (!valid);

    strncpy(email, tmp, max_len);

    free(tmp);
}

void input_read_positive_float(char *prompt, char *reject_prompt, float *dest) {
    bool valid;
    float tmp;

    do {
        fputs(prompt, stdout);
        scanf("%f", &tmp);
        getchar();

        /* vérifie le signe du réel saisi */
        valid = (tmp >= 0.0f);

        if (!valid) {
            log_warning(true, reject_prompt);
            move_cursor_up(1);
            erase_line();
            move_cursor_up(1);
            erase_line();
        }
    } while (!valid);

    *dest = tmp;
}

void input_read_positive_int(char *prompt, char *reject_prompt, int *dest) {
    bool valid;
    int tmp;

    do {
        fputs(prompt, stdout);
        scanf("%d", &tmp);
        getchar();

        /* vérifie le signe de l'entier saisi */
        valid = (tmp >= 0);

        if (!valid) {
            log_warning(true, reject_prompt);
            move_cursor_up(1);
            erase_line();
            move_cursor_up(1);
            erase_line();
        }
    } while (!valid);

    *dest = tmp;
}

bool input_confirm(const char *prompt) {
    char c;
    bool valid, confirm;

    do {
        /* saisir le choix */
        log_warning(false, "%s [o/N] ", prompt);
        scanf("%c", &c);

        switch (c) {
        /* ne confirme pas par défaut */
        case '\n':
            valid = true;
            confirm = false;
            break;
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

        if (!valid) {
            move_cursor_up(1);
            erase_line();
        }

        if (c != '\n') {
            input_flush_stdin();
        }
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

    /* vérifie si la chaine a une longueur comprise entre 0 exclus et 30 inclus */
    valid = ((len > 0) && (len <= 30));

    /* parcourir la chaîne de caractères */
    for (i = 0; i < len && valid; i++) {
        c = input[i];
        /* vérifier si le caractère actuel est
         * une lettre, un espace ou un tiret */
        valid = (isalpha(c) || (c == ' ') || (c == '-'));
    }

    if (!valid) {
        log_warning(
            true,
            "Le nom ne peut contenir que des lettres, espaces et tirets "
            "et doit faire entre 1 et 30 caractères",
            stdout
        );
        move_cursor_up(1);
        erase_line();
        move_cursor_up(1);
        erase_line();
    }

    return valid;
}

static bool input_validate_alphanum(char *input, size_t len) {
    bool valid;
    size_t i;
    char c;

    /* vérifie si la chaine a une longueur comprise entre 0 exclus et 30 inclus */
    valid = ((len > 0) && (len <= 30));

    /* parcourir la chaîne de caractères */
    for (i = 0; i < len && valid; i++) {
        c = input[i];
        /* vérifier si le caractère actuel est
         * alphanumérique, un espace ou un tiret */
        valid = (isalpha(c) || (c == ' ') || (c == '-') || isdigit(c));
    }

    if (!valid) {
        log_warning(
            true,
            "Le nom de rue ne peut contenir que des lettres, espaces, tirets et chiffres"
            "et doit faire entre 1 et 30 caractères",
            stdout
        );
        move_cursor_up(1);
        erase_line();
        move_cursor_up(1);
        erase_line();
    }

    return valid;
}

static bool input_validate_num(char *reject_prompt, char *input, size_t len, size_t exact_len) {
    bool valid;
    size_t i;

    /* vérifie si la chaine a la longueur demande */
    valid = (len == exact_len);

    for (i = 0; i < len && valid; i++) {
        /* vérifie si la chaîne saisie est un nombre */
        valid = isdigit(input[i]);
    }

    if (!valid) {
        log_warning(true, reject_prompt);
        move_cursor_up(1);
        erase_line();
        move_cursor_up(1);
        erase_line();
    }

    return valid;
}

static bool input_validate_email(char *reject_prompt, char *input, size_t len) {
    bool valid;

    (void)input;
    (void)len;

    /* toutes les adresses emails seront valides, bien évidemment ... */
    valid = true;

    if (!valid) {
        log_warning(true, reject_prompt);
        move_cursor_up(1);
        erase_line();
        move_cursor_up(1);
        erase_line();
    }

    return valid;
}
