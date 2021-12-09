// Fichier shm.h à rédiger
#include <stdnoreturn.h> // raler

#include "asem.h" // asem_t

struct vaccinodrome {
    int n;
    int m;
    int t;
    asem_t salle_attente;
};
typedef struct vaccinodrome vaccinodrome_t;

noreturn void raler(char *message);
int string_to_int(char *arg);
