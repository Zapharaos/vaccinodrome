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
    int fd = shm_open(FILE_NAME, O_RDWR, 0666);
    CHECK(fd); // existe pas

    struct stat sb;
    CHECK(fstat(fd, &sb));

    vaccinodrome_t *vac = (vaccinodrome_t *) mmap(NULL, sb.st_size,
                                    PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    NCHECK(vac);

    // // check si fermé
    // if(vac->status == FERME) exit(EXIT_FAILURE);

    // attend d'avoir une place de libre
    CHECK(asem_wait(&(vac->salle_attente)));

    // check si place s'est libérée mais que vaccinodrome a fermé entre temps
    // sinon wait avant 1er check si fermé
    if(vac->status == FERME)
    {
        CHECK(asem_post(&(vac->salle_attente))); // libère place
        exit(EXIT_FAILURE);
    }
    
    // semaphore ?
    vac->salle_count++; // nombre de patients dans la salle d'attente
    vac->pat_count++; // nombre de patients dans le vaccinodrome
    int id_patient; // id du patient

    CHECK(asem_wait(&(vac->edit_salle))); // une installation à la fois
    // il y a forcément au moins un patient car on a passé le wait ligne 29
    for(int i=0; i < (vac->n + vac->m); i++)
    {
        if(vac->patient[i].status == LIBRE)
        {
            vac->patient[i].status = OCCUPE;
            strncpy(vac->patient[i].nom, nom, MAX_NOMSEM + 1);
            asem_post(&(vac->edit_salle)); // fini de s'installer
            id_patient = i;
            printf("patient %s siege %d\n", nom, id_patient);
            break;
        }
    }

    CHECK(asem_post(&(vac->is_in_salle))); // libere place sale d'attente
    CHECK(asem_wait(&(vac->patient[id_patient].s_patient))); // attend medecin
    CHECK(asem_post(&(vac->salle_attente))); // libere place sale d'attente

    int id_medecin = vac->patient[id_patient].id_medecin; // recupere id_med
    fprintf(stdout, "patient %s medecin %d\n", nom, id_medecin);

    CHECK(asem_post(&(vac->patient[id_patient].s_medecin))); // rejoint médecin
    CHECK(asem_wait(&(vac->patient[id_patient].s_patient))); // attend fin vac

    // quitte le vaccinodrome
    // semaphore ?
    vac->pat_count--;
    vac->patient[id_patient].status = LIBRE;

    // dernier patient signale que les medecins peuvent partir
    if(vac->status == FERME && vac->pat_count == 0)
        CHECK(asem_post(&(vac->pat_vide)));
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