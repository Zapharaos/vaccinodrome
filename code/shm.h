// Fichier shm.h à rédiger
#include <stdnoreturn.h> // raler
#include <stdbool.h> // bool

#include "asem.h" // asem_t

struct vaccinodrome {
    bool statut;
    int med_count;
    asem_t vide;
    int pat_count;
    asem_t pat_vide;
    int n;
    int m;
    int t;
    int siege_count;
    asem_t salle_attente;
};
typedef struct vaccinodrome vaccinodrome_t;

noreturn void raler(char *message);
int string_to_int(char *arg);
