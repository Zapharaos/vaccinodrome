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

    vaccinodrome_t *vac = (vaccinodrome_t *) mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    // ferme
    vac->status = FERME;
    // int test;

    if(vac->med_count > 0)
    {
        for(int i=0; i < vac->m; i++) // puis sem post sem pour chaque médecin
        {
            asem_post(&(vac->is_in_salle)); // post trouverunnom
            // asem_getvalue(&(vac->is_in_salle), &test);
            // adebug(0, "m = %d - FERME(0) = %d && value = %d", i, vac->status, test);
        }
    }
    else if(vac->salle_count > 0)
    {
        adebug (0, "ici2 %d", vac->status);
        asem_wait(&(vac->fermer)); // dernier medecin previent ici post fermer
        for(int i=0; i < vac->m; i++) // puis sem post sem pour chaque médecin
            asem_post(&(vac->is_in_salle)); // post trouverunnom
    }

    if(vac->med_count != 0) // attendre que tout le monde soit rentré
        asem_wait(&(vac->vide));

    vac->n = 0;
    vac->m = 0;
    vac->t = 0;
    vac->med_count = 0;
    vac->pat_count = 0;

    asem_destroy (&(vac->vide));
    asem_destroy (&(vac->pat_vide));
    asem_destroy (&(vac->is_in_salle));
    asem_destroy (&(vac->salle_attente));
    asem_destroy (&(vac->edit_salle));

    for(int i=0; i < (vac->n+vac->m); i++)
    {
        asem_destroy(&(vac->patient[i].s_patient));
        asem_destroy(&(vac->patient[i].s_medecin));
    }

    if(munmap(vac, sb.st_size) == -1)
        raler("munmap");
    shm_unlink("/vaccinodrome");
}

int main (int argc, char *argv [])
{
    ainit(argv[0]);

    if(argc < 1 || argc >= 2)
    {
        fprintf(stderr, "usage: ./fermer\n");
        exit(EXIT_FAILURE);
    }

    fermer();
    return 0;
}
