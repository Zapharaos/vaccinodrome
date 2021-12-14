#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h> // flags
#include <sys/mman.h> // shm_open
#include <unistd.h> // ftruncate
#include <sys/stat.h> // fstat
#include <time.h> // usleep

#include "shm.h"

void medecin()
{
    int fd = shm_open(FILE_NAME, O_RDWR, 0666);
    CHECK(fd); // existe pas

    int lg = -1; // flags fonction, lg vaudra la taille de fd apres l'appel
    vaccinodrome_t *vac = get_vaccinodrome(fd, &lg);
    CHECK(close(fd)); // fermeture

    // debut section critique commune : lire le statut + entrer vaccinodrome
    CHECK(asem_wait(&(vac->edit_salle)));
    if(vac->status == FERME) // verifie si ferme
    {
        CHECK(asem_post(&(vac->edit_salle))); // fin section critique commune
        return;
    }
    else
        CHECK(asem_post(&(vac->edit_salle))); // fin section critique commune

    // debut section critique des medecins : entre dans le vaccinodrome
    CHECK(asem_wait(&(vac->medecin)));

    if(vac->m == vac->med_count) // + de medecins que de box existant : erreur
    {
        CHECK(asem_post(&(vac->medecin))); // fin section critique medecins
        raler("plus de medecins que de box");
    }
    int id_m = vac->med_count; // id du medecin
    vac->med_count++; // augmente de 1 le nombre de medecin

    CHECK(asem_post(&(vac->medecin))); // fin section critique des medecins

    int id_p; // id du patient

    while(1)
    {
        if(vac->status == FERME && vac->salle_count == 0) break;
 
        // post par patient qui entre ou par "fermer.c" pour m medecins
        CHECK(asem_wait(&(vac->salle_m)));

        if(vac->status == FERME && vac->salle_count == 0) break; // "fermer.c"

        // sinon, debut section critique commune : medecin recupere un patient
        CHECK(asem_wait(&(vac->edit_salle)));

        // il y a toujours au moins un patient car on a passé le wait ligne 47
        for(int i=0; i < (vac->n + vac->m); i++)
        {
            if(vac->patient[i].status == OCCUPE)
            {
                vac->patient[i].status = TRAITEMENT;
                vac->salle_count--;
                CHECK(asem_post(&(vac->edit_salle))); // fin section critique
                id_p = i;
                break;
            }
        }

        // hors critique car impossible de modifier si statut != LIBRE
        vac->patient[id_p].id_medecin = id_m;
        printf("medecin %d vaccine %s\n", id_m, vac->patient[id_p].nom);

        CHECK(asem_post(&(vac->patient[id_p].s_patient))); // cherche patient
        CHECK(asem_wait(&(vac->patient[id_p].s_medecin)));// attend patient box

        usleep(vac->t); // vaccination

        CHECK(asem_post(&(vac->patient[id_p].s_patient))); // libère le patient
    }

    // debut section critique commune : il ne doit rester aucun patient
    CHECK(asem_wait(&(vac->edit_salle)));

    if(vac->pat_count != 0) // il reste des patients dans le vaccinodrome
    {
        CHECK(asem_post(&(vac->edit_salle))); // fin section critique commune
        CHECK(asem_wait(&(vac->pat_vide))); // attend que les patients partent
    }
    else // il ne reste plus aucun patient
        CHECK(asem_post(&(vac->edit_salle))); // fin section critique commune

    // debut section critique des medecins : quitter le vaccinodrome
    CHECK(asem_wait(&(vac->medecin)));

    vac->med_count--;
    CHECK(asem_post(&(vac->pat_vide))); // libère les derniers médecins
    if(vac->med_count == 0) // dernier medecin : signale fermeture du vac
        CHECK(asem_post(&(vac->vide)));

    // fin section critique des medecins : medecin a quitte le vaccinodrome
    CHECK(asem_post(&(vac->medecin)));
}

int main (int argc, char *argv [])
{
    ainit(argv[0]);

    if(argc < 1 || argc >= 2)
    {
        fprintf(stderr, "usage: ./medecin\n");
        exit(EXIT_FAILURE);
    }

    medecin();
    return 0;
}