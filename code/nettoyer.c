// Fichier nettoyer.c à rédiger
#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h> // flags
#include <sys/mman.h> // shm_open
#include <unistd.h> // ftruncate
#include <sys/stat.h> // fstat

#include "shm.h"

void nettoyer()
{
    int fd = shm_open(FILE_NAME, O_RDWR, 0666);
    if(fd == -1) return; // deja supprime

    struct stat sb;
    CHECK(fstat(fd, &sb));

    vaccinodrome_t *vac = (vaccinodrome_t *) mmap(NULL, sb.st_size,
                                    PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    NCHECK(vac);
    
    CHECK(clean_file(vac, sb.st_size)); // clean
}

int main (int argc, char *argv [])
{
    ainit(argv[0]);

    if(argc < 1 || argc >= 2)
    {
        fprintf(stderr, "usage: ./nettoyer\n");
        exit(EXIT_FAILURE);
    }

    nettoyer();
    return 0;
}
