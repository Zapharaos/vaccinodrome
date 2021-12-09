// Fichier patient.c à rédiger
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h> // flags
#include <sys/mman.h> // shm_open
#include <unistd.h> // ftruncate
#include <sys/stat.h> // fstat

#include "shm.h"

void patient()
{
    int fd = shm_open("/vaccinodrome", O_RDWR, 0666);
    if(fd == -1) exit(EXIT_FAILURE);

    struct stat sb;
    if (fstat(fd, &sb) < 0)
        raler("Erreur lstat");

    ftruncate(fd , sizeof(vaccinodrome_t));
    vaccinodrome_t *vac = (vaccinodrome_t *) mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    // check si fermé
    if(vac->statut == false) exit(EXIT_FAILURE);

    // attend d'avoir une place de libre
    asem_wait(&(vac->salle_attente));

    // check si une place s'est libérée mais que le vaccinodrome a fermé entre temps
    // sinon wait avant check fermé
    if(vac->statut == false)
    {
        asem_post(&(vac->salle_attente)); // libère la place, peut-être que d'autres sont dans le même cas
        return;
    }

    // insérer nos données dans le tableau ===> id siège de ce patient = siege_count
    vac->pat_count++; // nb patients global dans le vaccinodrome
    vac->siege_count++; // nb sieges utilisés

    // attendre un médecin (comment récup id_med, check tous les box pour voir dans lequel on est)

    // libere place sale d'attente
    vac->siege_count--;
    asem_post(&(vac->salle_attente));

    // rejoindre médecin + se faire vacciner + libérer le médecin ?

    vac->pat_count--;

    if(vac->pat_count == 0) // signale que tous les patients sont partis
        asem_post(&(vac->pat_vide));
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
