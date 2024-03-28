#include <stdlib.h>
#include <stdbool.h>

void input_read_stdin(char **input, size_t *len);
bool input_validate_name(char *input, size_t len);
bool input_validate_phone(char *input, size_t len);
bool input_validate_email(char *input, size_t len);
bool input_validate_street_number(char *input, size_t len);
bool input_validate_zip_code(char *input, size_t len);
