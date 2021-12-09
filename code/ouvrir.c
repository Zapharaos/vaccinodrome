#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <fcntl.h> // flags
#include <sys/mman.h> // shm_open
#include <unistd.h> // ftruncate
#include <sys/stat.h> // fstat

#include "shm.h"

void ouvrir(int n, int m, int t)
{
    int fd = shm_open("/vaccinodrome", O_RDWR | O_EXCL | O_CREAT, 0666);

    // vérifier qu'il existe pas déjà + TESTER ERRNO
    if( fd == -1 ) raler("shm_open ouvrir");

    ftruncate(fd , sizeof(vaccinodrome_t*));
    vaccinodrome_t *vac = (vaccinodrome_t *) mmap(NULL, sizeof(vaccinodrome_t*), PROT_WRITE, MAP_SHARED, fd, 0);

    vac->statut = true;
    vac->n = n;
    vac->m = m;
    vac->t = t;
    vac->med_count = 0;
    vac->pat_count = 0; // nombre de patient total ou nombre de patient dans les box ?
    asem_init (&(vac->vide), "vide", 0, 0);
    asem_init (&(vac->pat_vide), "pat_vide", 0, 0);
    asem_init (&(vac->salle_attente), "wait_room", 0, vac->n);
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
