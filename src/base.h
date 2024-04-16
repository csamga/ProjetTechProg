#ifndef BASE_H_INCLUDED
#define BASE_H_INCLUDED

enum modes {
    mode_client,
    mode_supplier,
    mode_product,
    mode_sale,
    mode_quit
};

void base_register(enum modes mode);
void base_inspect(enum modes mode);
void base_modify(enum modes mode);
void base_delete(enum modes mode);

#endif
