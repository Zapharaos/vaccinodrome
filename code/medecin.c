// Fichier medecin.c à rédiger
#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h> // flags
#include <sys/mman.h> // shm_open
#include <unistd.h> // ftruncate
#include <sys/stat.h> // fstat

#include "shm.h"

void medecin()
{
    int fd = shm_open("/vaccinodrome", O_RDWR, 0666);
    if(fd == -1) exit(EXIT_FAILURE);

    struct stat sb;
    if (fstat(fd, &sb) < 0)
        raler("Erreur lstat");

    ftruncate(fd , sizeof(vaccinodrome_t));
    vaccinodrome_t *vac = (vaccinodrome_t *) mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    // check si c'est fermé
    if(vac->statut == false) return; //exit(EXIT_FAILURE);

    // box id value = med_count
    vac->med_count++;

    while(vac->statut == true) // && vac->nb_patient > 0 ?? et si deux médecins accèdent en même temps au même patient ?
    {
        // récupérer un patient -> num aléatoire entre 0 et siege_count
        // sleep(args->t);
        // libérer un patient
    }

    asem_wait(&(vac->pat_vide)); // attend que tous les patients soit partis

    vac->med_count--;

    if(vac->med_count == 0) // signale que tous les médecins sont partis
        asem_post(&(vac->vide));

    return;
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
