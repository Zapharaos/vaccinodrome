// Fichier fermer.c à rédiger
#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h> // flags
#include <sys/mman.h> // shm_open
#include <unistd.h> // ftruncate
#include <sys/stat.h> // fstat

#include "shm.h"

void fermer()
{
    int fd = shm_open(FILE_NAME, O_RDWR, 0666);
    CHECK(fd); // deja supprime

    struct stat sb;
    CHECK(fstat(fd, &sb));

    vaccinodrome_t *vac = (vaccinodrome_t *) mmap(NULL, sb.st_size,
                                    PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    NCHECK(vac);

    vac->status = FERME; // statut du vaccinodrome

    if(vac->med_count > 0) // si il reste encore des médecins
        for(int i=0; i < vac->m; i++) // pour chaque médecin
            CHECK(asem_post(&(vac->is_in_salle))); // post prévenir fermeture

    if(vac->med_count != 0) // attendre que tout le monde soit rentré
        CHECK(asem_wait(&(vac->vide)));

    CHECK(clean_file(vac, sb.st_size)); // clean
}

int main (int argc, char *argv [])
{
    ainit(argv[0]);

    if(argc < 1 || argc >= 2)
    {
        fprintf(stderr, "usage: ./fermer\n");
        exit(EXIT_FAILURE);
    }

    fermer();
    return 0;
}
