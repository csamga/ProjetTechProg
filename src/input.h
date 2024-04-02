#include <stdlib.h>
#include <stdbool.h>

int acquire_input(void);
bool validate_input(int choice, int first, int last);

void input_read_stdin(char **input, size_t *len);

void input_read_last_name(char *name, size_t max_len);
void input_read_first_name(char *name, size_t max_len);
void input_read_phone(char *phone, size_t max_len);
void input_read_email(char *email, size_t max_len);
void input_read_price(float *price);

bool input_validate_name(char *input, size_t len);
bool input_validate_phone(char *input, size_t len);
bool input_validate_email(char *input, size_t len);
bool input_validate_street_number(char *input, size_t len);
bool input_validate_zip_code(char *input, size_t len);
bool input_validate_price(float price);

bool input_confirm_delete(const char *message);

void input_flush_stdin(void);
