// Fichier nettoyer.c à rédiger
#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h> // flags
#include <sys/mman.h> // shm_open
#include <unistd.h> // ftruncate
#include <sys/stat.h> // fstat

#include "shm.h"

void nettoyer()
{
    int fd = shm_open("/freitagmatthieu", O_RDONLY, 0666);

    if(fd == -1) return; // deja supprime

    // struct stat sb;
    // if (fstat(fd, &sb) < 0)
    //     raler("Erreur lstat");

    // ftruncate(fd , sizeof(vaccinodrome_t));
    // vaccinodrome_t *vac = (vaccinodrome_t *) mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    
    // asem_destroy (&(vac->vide));
    // asem_destroy (&(vac->pat_vide));
    // asem_destroy (&(vac->salle_attente));

    // if(munmap(vac, sb.st_size) == -1)
    //     raler("munmap");

    // doit aussi post le semaphore ?

    shm_unlink("/freitagmatthieu");
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
