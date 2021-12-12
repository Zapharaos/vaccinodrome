// Fichier shm.h à rédiger
#include <stdnoreturn.h> // raler
#include <unistd.h> // usleep type
#include <sys/mman.h> // munmap

#include "asem.h" // asem_t

#define FILE_NAME "/matthieu_freitag"

#define CHECK(op) do { if (((op) == -1)) raler (#op); } while (0)
#define NCHECK(op) do { if (((op) == NULL)) raler (#op); } while (0)
#define TCHK(op) do { if ((errno=(op)) > 0) { raler (#op); } } while (0)

/** @enum vac_status
 *  @brief Cette enum decrit le statut du vaccinodrome
 */
enum vac_status
{
    FERME = 0,       /**< vaccinodrome est ferme */
    OUVERT = 1,      /**< vaccinodrome est ouvert */
};
typedef enum vac_status vac_status_t;

/** @enum pat_status
 *  @brief Cete enum decrit le statut d'un patient (siege/box)
 */
enum pat_status
{
    LIBRE = 0,       /**< le siege est libre */
    OCCUPE = 1,      /**< le siege est occupe */
    TRAITEMENT = 2   /**< le siege est un box : vaccination en cours */
};
typedef enum pat_status pat_status_t;

/** @struct patient
 *  @brief Cette structure represente un siege ou un box dans un vaccinodrome
 */
/** @var pat_status_t::status
 *  'status' contient le status du siege/box
 */
/** @var char*::nom
 *  'nom' contient le nom du patient actuel
 */
/** @var int::id_medecin
*  'id_medecin' contient l'id du medecin qui traite le patient actuel
*/
/** @var asem_t::s_patient
*  's_patient' contient le semaphore pour le patient
*/
/** @var asem_t::s_medecin
*  's_medecin' contient le semaphore pour le medecin
*/
struct patient {
    pat_status_t status;
    char nom[MAX_NOMSEM + 1];
    int id_medecin;
    asem_t s_patient;
    asem_t s_medecin;
};
typedef struct patient patient_t;

/** @struct vaccinodrome
 *  @brief Cette structure represente un vaccinodrome
 */
/** @var int::n
 *  'n' contient le nombre de sieges
 */
/** @var int::m
 *  'm' contient le nombre de box
 */
/** @var useconds_t::t
*  't' contient la duree d'une vaccination en millisecondes
*/
/** @var vac_status_t::status
*  'status' contient le status du vaccinodrome
*/
/** @var int::med_count
 *  'med_count' contient le nombre de médecin dans le vaccinodrome
 */
 /** @var int::pat_count
 *  'pat_count' contient le nombre de patient dans le vaccinodrome
 */
 /** @var int::salle_count
 *  'salle_count' contient le nombre de patient dans la salle d'attente
 */
/** @var asem_t::vide
*  'vide' contient un semaphore null indiquant que le vaccinodrome est vide
*/
/** @var asem_t::pat_vide
*  'pat_vide' contient un semaphore null indiquant qu'il ne reste de patients
*/
/** @var asem_t::salle_m
*  'salle_m' contient un semaphore null pour la salle d'attente (pour medecin)
*/
/** @var asem_t::salle_p
*  'salle_p' contient le semaphore à n pour la salle d'attente (pour patient)
*/
/** @var patient_t::patient
*  'patient' contient un tableau de patient (siege/box) de taille n+m
*/
struct vaccinodrome {
    int n;
    int m;
    useconds_t t;
    vac_status_t status;
    int med_count;
    int pat_count;
    int salle_count;
    asem_t vide;
    asem_t pat_vide;
    asem_t is_in_salle;
    asem_t salle_attente;
    asem_t edit_salle;
    patient_t patient[];
};
typedef struct vaccinodrome vaccinodrome_t;

/**
 * @fn      noreturn void raler(char *message)
 * @brief   Displays a message when an error has occured
 * @param   message     Message to display
 */
noreturn void raler(const char *message);

/**
 * @fn      int string_to_int(char *arg)
 * @brief   Transforms a given string to an int
 * @param   arg     String to transform
 * @return  String transformed to int
 */
int string_to_int(char *arg);

/**
 * @fn      int clean_file(vaccinodrome_t *vac, size_t lg)
 * @brief   Cleans shared memomry
 * @param   arg     Structure to clean
 * @param   lg      Size of the structure
 * @return  -1 if an error has occured, else 0
 */
int clean_file(vaccinodrome_t *vac, size_t lg);
