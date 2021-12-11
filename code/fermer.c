// Fichier fermer.c à rédiger
#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h> // flags
#include <sys/mman.h> // shm_open
#include <unistd.h> // ftruncate
#include <sys/stat.h> // fstat

#include "shm.h"

void fermer()
{
    int fd = shm_open("/vaccinodrome", O_RDWR, 0666);
    if(fd == -1) exit(EXIT_FAILURE);

    struct stat sb;
    if (fstat(fd, &sb) < 0)
        raler("Erreur lstat");

    ftruncate(fd , sizeof(vaccinodrome_t));
    vaccinodrome_t *vac = (vaccinodrome_t *) mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    // ferme
    vac->status = FERME;

    if(vac->med_count != 0) // attendre que tout le monde soit rentré
        asem_wait(&(vac->vide));

    vac->n = 0;
    vac->m = 0;
    vac->t = 0;
    vac->med_count = 0;
    vac->pat_count = 0;

    asem_destroy (&(vac->vide));
    asem_destroy (&(vac->pat_vide));
    asem_destroy (&(vac->salle_attente));

    if(munmap(vac, sb.st_size) == -1)
        raler("munmap");
    shm_unlink("/vaccinodrome");
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
