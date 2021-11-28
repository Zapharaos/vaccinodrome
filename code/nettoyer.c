// Fichier nettoyer.c à rédiger
#include <stdio.h>
#include <stdlib.h>

void nettoyer()
{

}

int main (int argc, char *argv [])
{
    (void) argv;

    if(argc < 1 || argc >= 2)
    {
        fprintf(stderr, "usage: ./nettoyer\n");
        exit(EXIT_FAILURE);
    }

    nettoyer();
    return 0;
}
