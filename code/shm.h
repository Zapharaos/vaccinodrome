// Fichier shm.h à rédiger
#include <stdnoreturn.h> // raler
#include <stdbool.h> // bool

#include "asem.h" // asem_t

/** @enum individu_type
 *  @brief This enum describes the type of un individu
 */
enum status
{
    LIBRE = 0,       /**< a packet has been sent, waiting for the ACK */
    OCCUPE = 1,      /**< correct ACK received, sending new packet */
    TRAITEMENT = 2
};
typedef enum status status_t;

struct patient {
    status_t status;
    char nom[MAX_NOMSEM + 1];
    int id_medecin;
    asem_t sem_pat;
    asem_t sem_med;
};
typedef struct patient patient_t;

struct vaccinodrome {
    bool statut;
    int med_count;
    int pat_count;
    asem_t vide;
    asem_t pat_vide;
    int n;
    int m;
    int t;
    asem_t salle_attente;
    asem_t patients;
    asem_t medecins;
    patient_t patient[];
};
typedef struct vaccinodrome vaccinodrome_t;

noreturn void raler(char *message);
int string_to_int(char *arg);
