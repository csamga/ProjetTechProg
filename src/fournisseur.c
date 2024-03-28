#include "fournisseur.h"

#include <string.h>

static short n_fournisseur = 0;

static struct fournisseur fournisseur_read(void);

void fournisseur_register(void) {
    FILE *fournisseur_db;
    struct fournisseur fournisseur;

    fournisseur_db = fopen("fournisseur_db.dat", "ab");

    if (fournisseur_db == NULL) {
        fprintf(stderr, "error: failed to open %s\n", "fournisseur_db.dat");
        return;
    }

    fournisseur = fournisseur_read();
    fournisseur.id = n_fournisseur++;

    fwrite(&fournisseur, sizeof fournisseur, 1, fournisseur_db);

    fclose(fournisseur_db);
}

void fournisseur_delete(void) {
    char name[50];
    FILE *old_fournisseur_db, *new_fournisseur_db;
    struct fournisseur tmp;

    old_fournisseur_db = fopen("founisseur_db.dat", "rb");

    fputs("Nom : ", stdout);
    fgets(name, sizeof name, stdin);

    if (old_fournisseur_db == NULL) {
        fprintf(stderr, "error : failed to open %s\n", "old_fournisseur_db.dat");
    }
    
    new_fournisseur_db = fopen("tmp_fournisseur_db.dat", "a");

    while (!feof(old_fournisseur_db)) {
        fread(&tmp, sizeof tmp, 1, old_fournisseur_db);
        if (strcmp(tmp.name, name) != 0) {
            fwrite(&tmp, sizeof tmp, 1, new_fournisseur_db);
        }  
    }
    
    fclose(old_fournisseur_db);
    fclose(new_fournisseur_db);

    remove("fournisseur_db.dat");
    rename("new_fournisseur_db.dat", "fournisseur_db.dat");
}


void fournisseur_inspect(void) {
    struct fournisseur fournisseur;
    char name[50];
    bool exists;

    fputs("Nom fournisseur : ", stdout);
    fgets(name, sizeof name, stdin);
    name[strcspn(name, "\n")] = '\0';

    fournisseur_search_by_name(name, &fournisseur, &exists);

    if (exists) {
        puts("Fournisseur informations:");
        printf("Fournisseur: %s\n", fournisseur.name);
        printf("Phone: %s\n", fournisseur.phone);
        printf("Email: %s\n", fournisseur.email);
        printf("Address: %s\n", fournisseur.address);
    }
}

void fournisseur_search_by_name( 
    const char *name,
    struct fournisseur *fournisseur,
    bool *exists)
{
    FILE *fournisseur_db;
    struct fournisseur tmp;

    fournisseur_db = fopen("fournisseur_db.dat", "rb");

    if (fournisseur_db == NULL) {
        fprintf(stderr, "error: failled to open %s\n", "fournisseur_db.dat");
        return;
    }

    *exists = false;

    while (!feof(fournisseur_db) && (!*exists)) {
        fread(&tmp, sizeof tmp, 1, fournisseur_db);
        *exists = (tmp.name == name);
    }

    if (exists) {
        *fournisseur = tmp;
    } else {
        fournisseur = NULL;
        printf("Le fournisseur nommé %s n'existe pas", name);
    }

    fclose(fournisseur_db);  
}


void fournisseur_search_by_id(
    const unsigned short id,
    struct fournisseur *fournisseur,
    bool *exists)
{
    FILE *fournisseur_db;
    struct fournisseur tmp;

    fournisseur_db = fopen("fournisseur_db.dat", "rb");

    if (fournisseur_db == NULL) {
        fprintf(stderr, "error: failed to open %s\n", "fournisseur_db.dat");
        return;
    }

    *exists = false;

    while (!feof(fournisseur_db) && !*exists) {
        fread(&tmp, sizeof tmp, 1, fournisseur_db);
        *exists = (tmp.id == id);
    }

    if (exists) {
        *fournisseur = tmp;
    } else {
        fournisseur = NULL;
        printf("Le fournisseur n°%hd n'existe pas", id);
    }

    fclose(fournisseur_db);
}


static struct fournisseur fournisseur_read(void) {
    struct fournisseur fournisseur;

    puts("Enter fournisseur informations:");

    fputs("Name: ", stdout);
    fgets(fournisseur.name, sizeof fournisseur.name, stdin);
    fournisseur.name[strcspn(fournisseur.name, "\n")] = '\0';

    fputs("Phone: ", stdout);
    fgets(fournisseur.phone, sizeof fournisseur.phone, stdin);
    fournisseur.phone[strcspn(fournisseur.phone, "\n")] = '\0';

    fputs("Email: ", stdout);
    fgets(fournisseur.email, sizeof fournisseur.email, stdin);
    fournisseur.email[strcspn(fournisseur.email, "\n")] = '\0';

    fputs("Address: ", stdout);
    fgets(fournisseur.address, sizeof fournisseur.address, stdin);
    fournisseur.address[strcspn(fournisseur.address, "\n")] = '\0';

    return fournisseur;
}
