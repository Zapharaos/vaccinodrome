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
    int fd = shm_open("/freitagmatthieu", O_RDWR, 0666);
    if(fd == -1) exit(EXIT_FAILURE);

    struct stat sb;
    if (fstat(fd, &sb) < 0)
        raler("Erreur lstat");

    vaccinodrome_t *vac = (vaccinodrome_t *) mmap(NULL, sb.st_size, 
                        PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    // check si c'est fermé, cool pas besoin de travailler aujourd'hui
    if(vac->status == FERME) return;

    // check si + de médecins que de boxs, je me suis trompé d'adresse
    if(vac->m <= vac->med_count) exit(EXIT_FAILURE);

    int id_p = 0; // id du patient
    int id_m = vac->med_count; // id du medecin
    vac->med_count++; // augmente de 1 le nombre de medecin

    while(1)
    {
        if(vac->status == FERME && vac->salle_count == 0) break; // fermeture
 
        // post : patient arrive ou fermeture pour m medecins
        asem_wait(&(vac->is_in_salle));

        if(vac->status == FERME && vac->salle_count == 0) break; // fermeture

        asem_wait(&(vac->edit_salle)); // un tirage au sort à la fois, critique
        // il y a forcément au moins un patient car on a passé le wait ligne 39
        for(id_p = 0; id_p < (vac->n + vac->m); id_p++) // pat_count ???? n+m ???
        {
            if(vac->patient[id_p].status == OCCUPE)
            {
                vac->patient[id_p].status = TRAITEMENT;
                vac->salle_count--;
                asem_post(&(vac->edit_salle)); // fin tirage au sort, critique
                vac->patient[id_p].id_medecin = id_m;
                printf("medecin %d vaccine %s\n", id_m, vac->patient[id_p].nom);
                break;
            }
        }

        asem_post(&(vac->patient[id_p].s_patient)); // cherche le patient
        asem_wait(&(vac->patient[id_p].s_medecin)); // attend patient dans box

        usleep(vac->t); // vaccination

        asem_post(&(vac->patient[id_p].s_patient)); // libère le patient

        if(vac->status == FERME && vac->salle_count == 0)
            asem_post(&(vac->fermer)); // post fermer
    }

    if(vac->pat_count != 0) // attend que tous les patients partent
        asem_wait(&(vac->pat_vide)); 

    vac->med_count--;

    if(vac->med_count == 0) // dernier signale que vac est vide -> fermeture
        asem_post(&(vac->vide));

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