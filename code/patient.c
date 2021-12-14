#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h> // flags
#include <sys/mman.h> // shm_open
#include <unistd.h> // ftruncate
#include <sys/stat.h> // fstat

#include "shm.h"

void patient(char *nom)
{
    int fd = shm_open(FILE_NAME, O_RDWR, 0666);
    CHECK(fd); // existe pas

    int lg = -1; // flags fonction, lg vaudra la taille de fd apres l'appel
    vaccinodrome_t *vac = get_vaccinodrome(fd, &lg);
    CHECK(close(fd)); // fermeture

    // attend d'avoir une place de libre dans la salle d'attente
    CHECK(asem_wait(&(vac->salle_p)));

    // debut section critique commune : lire le statut
    CHECK(asem_wait(&(vac->edit_salle)));
    if(vac->status == FERME) // verifie si c'est ferme : on repart 
    {
        CHECK(asem_post(&(vac->edit_salle))); // fin section critique commune
        CHECK(asem_post(&(vac->salle_p))); // libère la place 
        raler("patient.c : vac ferme");
    }
    else
        CHECK(asem_post(&(vac->edit_salle))); // fin section critique commune

    // debut section critique commune : entre dans le vaccinodrome
    CHECK(asem_wait(&(vac->edit_salle)));
    
    vac->salle_count++; // nombre de patients dans la salle d'attente
    vac->pat_count++; // nombre de patients dans le vaccinodrome

    // fin section critique commune
    CHECK(asem_post(&(vac->edit_salle)));

    int id_patient; // id du patient

    // debut section critique commune : patient s'installe dans la salle 
    CHECK(asem_wait(&(vac->edit_salle)));

    // il y a au moins un siege de libre car on a passé le wait ligne 25
    for(int i=0; i < (vac->n + vac->m); i++)
    {
        if(vac->patient[i].status == LIBRE)
        {
            vac->patient[i].status = OCCUPE;
            CHECK(asem_post(&(vac->edit_salle))); // fini de s'installer
            id_patient = i;
            break;
        }
    }

    // hors critique car impossible de modifier si statut != LIBRE
    strncpy(vac->patient[id_patient].nom, nom, MAX_NOMSEM + 1);
    printf("patient %s siege %d\n", nom, id_patient);

    CHECK(asem_post(&(vac->salle_m))); // installe : previent les medecins 
    CHECK(asem_wait(&(vac->patient[id_patient].s_patient))); // attend medecin
    CHECK(asem_post(&(vac->salle_p))); // libere place sale d'attente

    // hors critique car impossible de modifier si statut != LIBRE
    int id_medecin = vac->patient[id_patient].id_medecin; // recupere id_med
    fprintf(stdout, "patient %s medecin %d\n", nom, id_medecin);

    CHECK(asem_post(&(vac->patient[id_patient].s_medecin))); // rejoint médecin
    CHECK(asem_wait(&(vac->patient[id_patient].s_patient))); // attend fin vac

    // debut section critique commune : patient quitte le vaccinodrome
    CHECK(asem_wait(&(vac->edit_salle)));

    vac->pat_count--;
    vac->patient[id_patient].status = LIBRE;

    if(vac->status == FERME && vac->pat_count == 0) // dernier patient
    {
        CHECK(asem_post(&(vac->dernier))); // signale fin à fermer.c
        CHECK(asem_post(&(vac->pat_vide))); // signale medecins de partir
    }

    // fin section critique commune : patient a quitter le vaccinodrome
    CHECK(asem_post(&(vac->edit_salle)));
}

int main (int argc, char *argv [])
{
    ainit(argv[0]);

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

    patient(argv[1]);

    return 0;
}