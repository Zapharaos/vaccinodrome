// Fichier fermer.c à rédiger
#include <stdio.h>
#include <stdlib.h>

void fermer()
{

}

int main (int argc, char *argv [])
{
    (void) argv;

    if(argc < 1 || argc >= 2)
    {
        fprintf(stderr, "usage: ./fermer\n");
        exit(EXIT_FAILURE);
    }

    fermer();
    return 0;
}
