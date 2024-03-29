#ifndef BASE_H_INCLUDED
#define BASE_H_INCLUDED

enum modes {
    MODE_CLIENT,
    MODE_SUPPLIER,
    MODE_PRODUCT,
    MODE_SALE,
    MODE_QUIT
};

void base_register(enum modes mode);
void base_inspect(enum modes mode);
void base_modify(enum modes mode);
void base_delete(enum modes mode);

#endif
