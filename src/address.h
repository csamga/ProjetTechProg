#ifndef ADDRESS_H_INCLUDED
#define ADDRESS_H_INCLUDED

struct address {
    unsigned short num;
    char name[32];
    char zip_code[6];
    char city[32];
};

void address_read(struct address *address);
void address_inspect(const struct address *address);

#endif
