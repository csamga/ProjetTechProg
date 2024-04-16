#include <stdlib.h>
#include <stdbool.h>

int menu_acquire_input(void);
bool menu_validate_input(int choice, int first, int last);

void input_read_alpha(char *prompt, char *dest, size_t max_len);
void input_read_num(char *prompt, char *reject_prompt, char *dest, size_t max_len, size_t exact_len);
void input_read_email(char *prompt, char *reject_prompt, char *email, size_t max_len);
void input_read_positive_float(char *prompt, char *reject_prompt, float *dest);
void input_read_positive_int(char *prompt, char *reject_prompt, int *dest);
void input_read_street(char *prompt, char *dest, size_t max_len);

bool input_confirm_delete(const char *message);

void input_flush_stdin(void);
