#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h> // flags
#include <sys/mman.h> // shm_open
#include <unistd.h> // ftruncate
#include <sys/stat.h> // fstat

#include "shm.h"

void fermer()
{
    int fd = shm_open(FILE_NAME, O_RDWR, 0666);
    CHECK(fd); // deja supprime

    int lg = -1; // flags fonction, lg vaudra la taille de fd apres l'appel
    vaccinodrome_t *vac = get_vaccinodrome(fd, &lg);

    // debut section critique commune : modifier statut + prevenir medecins
    CHECK(asem_wait(&(vac->edit_salle)));

    vac->status = FERME; // le vaccinodrome ferme

    if(vac->pat_count > 0)
    {
        CHECK(asem_post(&(vac->edit_salle))); // fin section critique
        CHECK(asem_wait(&(vac->dernier))); // dernier patient
    }
    else
        CHECK(asem_post(&(vac->edit_salle))); // fin section critique

    // debut section critique commune : modifier statut + prevenir medecins
    CHECK(asem_wait(&(vac->edit_salle)));

    if(vac->med_count > 0) // si il reste encore des médecins : prevenir
    {
        CHECK(asem_post(&(vac->edit_salle))); // fin section critique

        for(int i=0; i < vac->m; i++) // pour chaque médecin
            CHECK(asem_post(&(vac->salle_m))); // prévenir de la fermeture

        // debut section critique commune : acces med_count + attendre fin
        CHECK(asem_wait(&(vac->edit_salle)));

        if(vac->med_count != 0) // si il reste encore des medecins
        {
            CHECK(asem_post(&(vac->edit_salle))); // fin section critique
            CHECK(asem_wait(&(vac->vide))); // attendre medecins partent
        } else // sinon, pas de médecin à attendre
            CHECK(asem_post(&(vac->edit_salle))); // fin section critique
    }
    else // sinon, pas de médecin à attendre
        CHECK(asem_post(&(vac->edit_salle))); // fin section critique

    CHECK(clean_file(vac, lg)); // clean
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
