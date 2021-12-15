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

    // debut section critique commune : modifier statut + vérifier nb patients
    CHECK(asem_wait(&(vac->edit_salle)));

    vac->status = FERME; // le vaccinodrome ferme
    int temp_pc = vac->pat_count;
    int temp_mc = vac->med_count;
    int temp_m = vac->m;

    // fin section critique commune
    CHECK(asem_post(&(vac->edit_salle)));

    if(temp_pc > 0 && temp_mc == 0) // aucun medecin n'est venu
    {
        for(int i=0; i < temp_pc; i++)
            CHECK(asem_post(&(vac->patient[i].s_patient))); // ferme patients

        // debut section critique commune : recuperer compteur, a pu changer
        CHECK(asem_wait(&(vac->edit_salle)));
        temp_pc = vac->pat_count;
        CHECK(asem_post(&(vac->edit_salle))); // fin section critique

        if(temp_pc != 0) // si il reste encore des medecins
            CHECK(asem_wait(&(vac->dernier))); // attend le dernier patient
    }
    else if(temp_mc > 0) // si il reste encore des médecins : prevenir
    {
        for(int i=0; i < temp_m; i++) // pour chaque médecin
            CHECK(asem_post(&(vac->salle_m))); // prévenir de la fermeture

        // debut section critique commune : recuperer compteur, a pu changer
        CHECK(asem_wait(&(vac->edit_salle)));
        temp_mc = vac->med_count;
        CHECK(asem_post(&(vac->edit_salle))); // fin section critique

        if(temp_mc != 0) // si il reste encore des medecins
            CHECK(asem_wait(&(vac->vide))); // attendre medecins partent
    }

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
