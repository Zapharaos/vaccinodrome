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
    int fd = shm_open("/vaccinodrome", O_RDWR, 0666);
    if(fd == -1) exit(EXIT_FAILURE);

    struct stat sb;
    if (fstat(fd, &sb) < 0)
        raler("Erreur lstat");

    vaccinodrome_t *vac = (vaccinodrome_t *) mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    // check si c'est fermé
    if(vac->status == FERME) return; //exit(EXIT_FAILURE);

    if(vac->m == vac->med_count) exit(EXIT_FAILURE);

    int id_medecin = vac->med_count;
    int id_patient;

    vac->med_count++;

    while(1)
    {
        adebug (0, "FERME(0) = %d && salle_count = %d", vac->status, vac->salle_count);
        if(vac->status == FERME && vac->salle_count == 0) break;
 
        asem_wait(&(vac->is_in_salle));

        adebug (0, "FERME(0) = %d && salle_count = %d", vac->status, vac->salle_count);
        if(vac->status == FERME && vac->salle_count == 0) break;

        asem_wait(&(vac->edit_salle)); // un tirage au sort à la fois
        for(int i=0; i < (vac->n + vac->m); i++) // pat_count ???? n+m ???
        {
            if(vac->patient[i].status == OCCUPE)
            {
                vac->patient[i].status = TRAITEMENT;
                vac->salle_count--;
                fprintf(stdout, "medecin %d vaccine %s\n", id_medecin, vac->patient[i].nom);
                id_patient = i;
                asem_post(&(vac->edit_salle)); // fin tirage au sort
                break;
            }
        }

        vac->patient[id_patient].id_medecin = id_medecin;

        asem_post(&(vac->patient[id_patient].s_patient)); // cherche le patient
        asem_wait(&(vac->patient[id_patient].s_medecin)); // attend que patient arrive dans la salle

        usleep(vac->t); // pas certain que le cast soit correct ici

        asem_post(&(vac->patient[id_patient].s_patient)); // libère le patient
        // asem_post(&(vac->patient[id_patient]->medecin)); // libère médecin ???

        if(vac->status == FERME && vac->salle_count == 0)
        {
            adebug(1, "dernier patient vient d'être traité %d", id_patient);
            asem_post(&(vac->fermer)); // post fermer
        }
    }

    adebug(1, "médecin %d attend que tous les patients partent, reste %d", id_medecin, vac->pat_count);

    if(vac->pat_count != 0) // attend que tous les patients soit partis
        asem_wait(&(vac->pat_vide)); 

    vac->med_count--;
    adebug(1, "médecin %d vient de partir, reste %d", id_medecin, vac->med_count);

    if(vac->med_count == 0) // le dernier signale que tous les médecins sont partis
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