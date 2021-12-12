// Fichier shm.h à rédiger
#include <stdnoreturn.h> // raler
#include <unistd.h> // usleep type

#include "asem.h" // asem_t

/** @enum individu_type
 *  @brief This enum describes the type of un individu
 */
enum vac_status
{
    FERME = 0,       /**< a packet has been sent, waiting for the ACK */
    OUVERT = 1,      /**< correct ACK received, sending new packet */
};
typedef enum vac_status vac_status_t;

/** @enum individu_type
 *  @brief This enum describes the type of un individu
 */
enum pat_status
{
    LIBRE = 0,       /**< a packet has been sent, waiting for the ACK */
    OCCUPE = 1,      /**< correct ACK received, sending new packet */
    TRAITEMENT = 2
};
typedef enum pat_status pat_status_t;

struct patient {
    pat_status_t status;
    char nom[MAX_NOMSEM + 1];
    int id_medecin;
    asem_t s_patient;
    asem_t s_medecin;
};
typedef struct patient patient_t;

struct vaccinodrome {
    vac_status_t status;
    asem_t fermer;
    int med_count;
    int pat_count;
    int salle_count;
    asem_t vide;
    asem_t pat_vide;
    int n;
    int m;
    useconds_t t;
    asem_t is_in_salle;
    asem_t salle_attente;
    asem_t edit_salle;
    // asem_t medecins;
    // patient_t patient[];
};
typedef struct vaccinodrome vaccinodrome_t;

noreturn void raler(char *message);
int string_to_int(char *arg);
