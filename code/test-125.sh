#!/bin/sh

#
# Test d'une session basique : un patient, pas de médecin
#

. ./ftest.sh

ATT=1                   # nb de sièges dans la salle d'attente
MED=1                   # nb de médecins
TVAX=0                  # durée de vaccination

# Nettoyer pour éviter des suprises
./nettoyer > $TMP.n 2>&1                || fail "Erreur au nettoyage préalable"

# Ouvrir le vaccinodrome
./ouvrir $ATT $MED $TVAX > $TMP.o 2>&1  || fail "Erreur à l'ouverture"

# Un patient arrive
./patient toto > $TMP.p 2>&1 &
PID_P=$!

msleep $MARGE

ps_existe $PID_P "patient devrait toujours être là"

# Fermer le vaccinodrome. Journee de greve nationale ?
./fermer > $TMP.f 2>&1 &
PID_F=$!

# Laisser le temps de terminer
msleep $MARGE

ps_termine $PID_P "patient aurait dû se terminer"
wait $PID_P                             || fail "Erreur du patient"

ps_termine $PID_F "fermer aurait dû se terminer"
wait $PID_F                             || fail "Erreur à la fermeture"

# On laisse tout bien propre à la fin
./nettoyer > $TMP 2>&1                  || fail "Erreur au nettoyage final"

logs_aux
echo "ok"
exit 0
