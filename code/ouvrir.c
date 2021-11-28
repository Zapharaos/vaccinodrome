// Fichier ouvrir.c à rédiger
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "shm.h"

void ouvrir(int n, int m, int t)
{
    (void) n;
    (void) m;
    (void) t;
}

int main (int argc, char *argv [])
{

    if(argc < 4 || argc >= 5)
    {
        fprintf(stderr, "usage: ./ouvrir <n> <m> <t>\n");
        exit(EXIT_FAILURE);
    }

    int n = string_to_int(argv[1]);
    int m = string_to_int(argv[2]);
    int t = string_to_int(argv[3]);

    if(n < 1 || m < 1 || t < 0)
    {
        fprintf(stderr, "usage: ./ouvrir n>0 && m>0 && t>=0\n");
        exit(EXIT_FAILURE);
    }

    ouvrir(n, m, t);
    return 0;
}
