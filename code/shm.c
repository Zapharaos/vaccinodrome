// Fichier shm.c à rédiger
#include "shm.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>

noreturn void raler(const char *message)
{
    perror(message);
    exit(1);
}

int string_to_int(char *arg)
{
    // variables
    char *endptr, *str;
    str = arg;

    errno = 0;
    long n = strtol(str, &endptr, 10);

    // check : error
    if ((errno == ERANGE && (n == LONG_MAX || n == LONG_MIN))
        || (errno != 0 && n == 0))
    {
        raler("strtol");
    }

    // if : not found
    if (endptr == str)
        raler("string_to_int nothing found");

    // cast to int (use of signed values later)
    return (int) n;
}

int clean_file(vaccinodrome_t *vac, size_t lg)
{
    CHECK(asem_destroy(&(vac->vide)));
    CHECK(asem_destroy(&(vac->pat_vide)));
    CHECK(asem_destroy(&(vac->is_in_salle)));
    CHECK(asem_destroy(&(vac->salle_attente)));
    CHECK(asem_destroy(&(vac->edit_salle)));

    for(int i=0; i < (vac->n+vac->m); i++)
    {
        CHECK(asem_destroy(&(vac->patient[i].s_patient)));
        CHECK(asem_destroy(&(vac->patient[i].s_medecin)));
    }

    CHECK(munmap(vac, lg));
    CHECK(shm_unlink(FILE_NAME));

    return 0;
}