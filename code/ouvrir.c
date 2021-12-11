#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <fcntl.h> // flags
#include <sys/mman.h> // shm_open
#include <unistd.h> // ftruncate
#include <sys/stat.h> // fstat

#include "shm.h"

void ouvrir(int n, int m, int t)
{
    int fd = shm_open("/vaccinodrome", O_RDWR | O_EXCL | O_CREAT, 0666);

    // vérifier qu'il existe pas déjà + TESTER ERRNO
    if( fd == -1 ) raler("shm_open ouvrir");

    ftruncate(fd , sizeof(vaccinodrome_t*));
    vaccinodrome_t *vac = (vaccinodrome_t *) mmap(NULL, sizeof(vaccinodrome_t*) + sizeof(patient_t [n+m]), PROT_WRITE, MAP_SHARED, fd, 0);

    vac->n = n; // nombre de sièges
    vac->m = m; // nombre de médecins
    vac->t = t; // temps de la vaccination

    vac->status = OUVERT; // statut du vaccinodrome
    vac->med_count = 0; // nombre de médecins actifs
    vac->pat_count = 0; // nombre de patients dans la salle d'attente + dans les box
    vac->salle_count = 0; // nombre de patients dans la salle d'attente

    asem_init (&(vac->vide), "vide", 0, 0);
    asem_init (&(vac->pat_vide), "pat_vide", 0, 0);

    asem_init (&(vac->salle_attente), "salle_att", 0, vac->n); // salle d'attente du vaccinodrome
    asem_init (&(vac->patients), "patient", 0, 1);
    asem_init (&(vac->medecins), "medecin", 0, 1);

    for(int i=0; i < (n+m); i++)
    {
        vac->patient[i].status = LIBRE;
        asem_init(&(vac->patient[i].sem_pat), "siege_pat", 0, 0);
        asem_init(&(vac->patient[i].sem_med), "siege_med", 0, 0);
    }
}

int main (int argc, char *argv [])
{

    if(argc < 4 || argc >= 5)
    {
        fprintf(stderr, "usage: ./ouvrir <n> <m> <t>\n");
        exit(EXIT_FAILURE);
    }

    int n = string_to_int(argv[1]);
    int m = string_to_int(argv[2]);
    int t = string_to_int(argv[3]);

    if(n < 1 || m < 1 || t < 0)
    {
        fprintf(stderr, "usage: ./ouvrir n>0 && m>0 && t>=0\n");
        exit(EXIT_FAILURE);
    }

    ouvrir(n, m, t);
    return 0;
}
