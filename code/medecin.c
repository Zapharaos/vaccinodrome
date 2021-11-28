// Fichier medecin.c à rédiger
#include <stdio.h>
#include <stdlib.h>

void medecin()
{

}

int main (int argc, char *argv [])
{
    (void) argv;

    if(argc < 1 || argc >= 2)
    {
        fprintf(stderr, "usage: ./medecin\n");
        exit(EXIT_FAILURE);
    }

    medecin();
    return 0;
}
