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
    int fd = shm_open(FILE_NAME, O_RDWR | O_EXCL | O_CREAT, 0666);
    CHECK(fd); // existe deja

    int lg = sizeof(vaccinodrome_t) + sizeof(patient_t)*(n+m);
    vaccinodrome_t *vac = get_vaccinodrome(fd, &lg); // ptr pour d'autres cas

    vac->n = n; // nombre de sièges
    vac->m = m; // nombre de médecins
    vac->t = t * 1000; // temps de la vaccination en microsecondes

    vac->status = OUVERT; // statut du vaccinodrome
    vac->med_count = 0; // nombre de médecins actifs
    vac->pat_count = 0; // nombre de patients dans la salle d'attente + box
    vac->salle_count = 0; // nombre de patients dans la salle d'attente

    // post med_count = pat_count = 0
    CHECK(asem_init(&(vac->vide), "vide", 1, 0));
    // post pat_count = 0
    CHECK(asem_init(&(vac->pat_vide), "pat_vide", 1, 0));
    // salle d'attente du vaccinodrome côté médecin
    CHECK(asem_init(&(vac->salle_m), "in_salle", 1, 0));
    // salle d'attente du vaccinodrome côté patient
    CHECK(asem_init(&(vac->salle_p), "salle_att", 1, vac->n));
    // sections critiques communes (medecins, patient, fermer)
    CHECK(asem_init(&(vac->edit_salle), "edit_salle", 1, 1));
    // sections critiques des medecins 
    CHECK(asem_init(&(vac->medecin), "med", 1, 1));

    CHECK(asem_init(&(vac->dernier), "dernier", 1, 0));

    for(int i=0; i < (n+m); i++) // nombre max de patients en meme temps : n+m
    {
        vac->patient[i].status = LIBRE;
        CHECK(asem_init(&(vac->patient[i].s_patient), "patient", 1, 0));
        CHECK(asem_init(&(vac->patient[i].s_medecin), "medecin", 1, 0));
    }
}

int main (int argc, char *argv [])
{

    ainit(argv[0]);

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
