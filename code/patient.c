// Fichier patient.c à rédiger
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void patient()
{

}

int main (int argc, char *argv [])
{

    if(argc < 2 || argc >= 3)
    {
        fprintf(stderr, "usage: ./patient <nom>\n");
        exit(EXIT_FAILURE);
    }
    else if (strcmp(argv[1], "") == 0)
    {
        fprintf(stderr, "usage: ./patient <nom> non vide\n");
        exit(EXIT_FAILURE);
    }
    else if (strlen(argv[1]) > 10)
    {
        fprintf(stderr, "usage: ./patient <nom> non vide\n");
        exit(EXIT_FAILURE);
    }

    patient();
    return 0;
}
