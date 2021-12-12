// Fichier patient.c à rédiger
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
    int fd = shm_open("/vaccinodrome", O_RDWR, 0666);
    if(fd == -1) exit(EXIT_FAILURE);

    struct stat sb;
    if (fstat(fd, &sb) < 0)
        raler("Erreur lstat");

    vaccinodrome_t *vac = (vaccinodrome_t *) mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    // check si fermé
    if(vac->status == FERME) exit(EXIT_FAILURE);

    // attend d'avoir une place de libre
    asem_wait(&(vac->salle_attente));

    // check si une place s'est libérée mais que le vaccinodrome a fermé entre temps
    // sinon wait avant 1er check si fermé
    if(vac->status == FERME)
    {
        asem_post(&(vac->salle_attente)); // libère la place, peut-être que d'autres sont dans le même cas
        exit(EXIT_FAILURE);
    }

    // asem_post(&(vac->vide)); ?
    
    vac->salle_count++;
    vac->pat_count++; // nb patients global dans le vaccinodrome
    int id_patient;

    // insérer nos données dans le tableau ===> id siège de ce patient = siege_count
    asem_wait(&(vac->patients)); // une installation à la fois
    for(int i=0; i < (vac->n + vac->m); i++) // pat_count ???? n+m ???
    {
        if(vac->patient[i].status == LIBRE)
        {
            vac->patient[i].status = OCCUPE;
            // vac->patient[i].nom = nom;
            strncpy(vac->patient[i].nom, nom, MAX_NOMSEM + 1);
            id_patient = i;
            asem_post(&(vac->patients)); // fini de s'installer
            break;
        }
    }

    asem_post(&(vac->trouverunnom)); 
    asem_wait(&(vac->patient[id_patient].patient)); // attend que medecin cherche patient
    // vac->salle_count--;
    asem_post(&(vac->salle_attente)); // libere place sale d'attente

    int id_medecin = vac->patient[id_patient].id_medecin;
    printf("%d %d\n", id_patient, id_medecin);

    asem_post(&(vac->patient[id_patient].medecin)); // rejoint médecin dans la salle d'attente // necessaire ?

    asem_wait(&(vac->patient[id_patient].patient)); // attend fin vaccination

    vac->pat_count--;
    vac->patient[id_patient].status = LIBRE;
    fprintf(stdout, "P\n");

    if(vac->status == FERME && vac->pat_count == 0) // signale que tous les patients sont partis (quand ferme)
        asem_post(&(vac->pat_vide));
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