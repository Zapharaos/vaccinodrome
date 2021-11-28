// Fichier shm.c à rédiger
#include "shm.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>

noreturn void raler(char *message)
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