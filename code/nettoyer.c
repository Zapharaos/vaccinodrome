#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h> // flags
#include <sys/mman.h> // shm_open
#include <unistd.h> // ftruncate
#include <sys/stat.h> // fstat

#include "shm.h"

void nettoyer()
{
    int fd = shm_open(FILE_NAME, O_RDWR, 0666);
    if(fd == -1) return; // deja supprime

    int lg = -1; // flags fonction, lg vaudra la taille de fd apres l'appel
    vaccinodrome_t *vac = get_vaccinodrome(fd, &lg);
    
    CHECK(clean_file(vac, lg)); // clean
}

int main (int argc, char *argv [])
{
    ainit(argv[0]);

    if(argc < 1 || argc >= 2)
    {
        fprintf(stderr, "usage: ./nettoyer\n");
        exit(EXIT_FAILURE);
    }

    nettoyer();
    return 0;
}
