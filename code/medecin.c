// Fichier medecin.c à rédiger
#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h> // flags
#include <sys/mman.h> // shm_open
#include <unistd.h> // ftruncate
#include <sys/stat.h> // fstat
#include <time.h> // random number

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

    int id_medecin = vac->med_count;
    int id_patient;

    vac->med_count++;

    // medecin random prévient patient, patient choisit un box random et s'installe ?

    while(vac->pat_count > 0) // tant que patient à traiter
    {
        // et si deux médecins accèdent en même temps au même patient ? => semaphore
        // asem_wait(&(vac->medecin)); // un tirage au sort à la fois
        // do {
        //     srand(time(NULL));
        //     patient = rand() % vac->n; // id patient random dans la salle d'attente
        // } while(vac->siege[patient]->status == LIBRE);
        // asem_post(&(vac->medecin)); // fin tirage au sort

        asem_wait(&(vac->medecins)); // un tirage au sort à la fois
        for(int i=0; i < (vac->n + vac->m); i++) // pat_count ???? n+m ???
        {
            if(vac->patient[i].status == OCCUPE)
            {
                vac->patient[i].status = TRAITEMENT;
                asem_post(&(vac->medecins)); // fin tirage au sort
                printf("%s\n", vac->patient[i].nom);
                id_patient = i;
                break;
            }
        }

        vac->patient[id_patient].id_medecin = id_medecin;

        asem_post(&(vac->patient[id_patient].sem_pat)); // cherche le patient
        asem_wait(&(vac->patient[id_patient].sem_med)); // attend que patient arrive dans la salle

        usleep((useconds_t) vac->t); // pas certain que le cast soit correct ici

        asem_post(&(vac->patient[id_patient].sem_pat)); // libère le patient
        // asem_post(&(vac->patient[id_patient]->sem_med)); // libère médecin ???
    }

    asem_wait(&(vac->pat_vide)); // attend que tous les patients soit partis

    vac->med_count--;

    if(vac->med_count == 0) // le dernier signale que tous les médecins sont partis
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
