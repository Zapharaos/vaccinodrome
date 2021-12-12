// Fichier medecin.c à rédiger
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

    struct stat sb;
    CHECK(fstat(fd, &sb));

    vaccinodrome_t *vac = (vaccinodrome_t *) mmap(NULL, sb.st_size,
                                    PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    NCHECK(vac);

    // check si c'est fermé, cool pas besoin de travailler aujourd'hui
    if(vac->status == FERME) return;

    // check si + de médecins que de boxs, je me suis trompé d'adresse
    // semaphore ?
    if(vac->m == vac->med_count) raler("plus de medecins que de box");

    int id_p; // id du patient

    // semaphore ?
    int id_m = vac->med_count; // id du medecin
    vac->med_count++; // augmente de 1 le nombre de medecin

    while(1)
    {
        if(vac->status == FERME && vac->salle_count == 0) break; // fermeture
 
        // post : patient arrive ou fermeture pour m medecins
        CHECK(asem_wait(&(vac->is_in_salle)));

        if(vac->status == FERME && vac->salle_count == 0) break; // fermeture

        CHECK(asem_wait(&(vac->edit_salle))); // un tirage au sort à la fois
        // il y a forcément au moins un patient car on a passé le wait ligne 40
        for(int i=0; i < (vac->n + vac->m); i++)
        {
            if(vac->patient[i].status == OCCUPE)
            {
                vac->patient[i].status = TRAITEMENT;
                vac->salle_count--;
                CHECK(asem_post(&(vac->edit_salle))); // fin tirage au sort
                id_p = i;
                break;
            }
        }

        vac->patient[id_p].id_medecin = id_m;
        printf("medecin %d vaccine %s\n", id_m, vac->patient[id_p].nom);

        CHECK(asem_post(&(vac->patient[id_p].s_patient))); // cherche patient
        CHECK(asem_wait(&(vac->patient[id_p].s_medecin)));// attend patient box

        usleep(vac->t); // vaccination

        CHECK(asem_post(&(vac->patient[id_p].s_patient))); // libère le patient
    }

    if(vac->pat_count != 0) // attend que tous les patients partent
        CHECK(asem_wait(&(vac->pat_vide))); 

    // quitte le vaccinodrome
    // semaphore ?
    vac->med_count--;

    if(vac->med_count == 0) // dernier medecin signale que le vac peut fermer
        CHECK(asem_post(&(vac->vide)));

    return;
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