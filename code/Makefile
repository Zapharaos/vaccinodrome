#
# Attention : vous ne devez pas modifier ce fichier.
#
# Ce Makefile contient les cibles suivantes :
#
# all		: compile les programmes
# clean		: supprime les fichiers générés automatiquement
# coverage	: compile les programmes pour mesurer la couverture de code
# test		: lance tous les tests (scripts shell test-xxx.sh)
# gcov		: génère les rapports de couverture (à lancer après avoir
#		  lancé les cibles 'coverage' et 'test').
#		  Résultats dans *.gcov
# ctags		: génère un fichier tags pour la navigation avec vim.
#		  (voir http://usevim.com/2013/01/18/tags/)
# rapport.pdf	: génère le rapport à partir du modèle rapport.tex
#		  (optionnel : vous pouvez rédiger le rapport avec
#		  l'outil que vous voulez, du moment que vous déposez
#		  un fichier PDF sur Moodle)
# devoir.tgz	: construit le fichier devoir.tgz que vous devez déposer
#		  sur Moodle
#
# De plus, les cibles supplémentaires suivantes sont fournies pour
# simplifier les tâches répétitives :
#
# couverture-et-tests	: automatise les tests avec rapport de couverture
#

COV = -coverage

CFLAGS = -g -Wall -Wextra -Werror $(COVERAGE)
LDLIBS = -g -lpthread -lrt

# Les fichiers attendus pour le rendu sur le devoir Moodle
DEVOIR = \
	shm.c shm.h \
	ouvrir.c fermer.c nettoyer.c medecin.c patient.c \
	rapport.pdf

PROGS = ouvrir fermer nettoyer medecin patient

all: $(PROGS)

#
# Vous devez créer un fichier shm.c contenant les fonctions communes
# à au moins deux programmes et un fichier shm.h pour vos définitions.
#

$(PROGS): asem.o asem.h shm.o

shm.o: shm.h asem.h
asem.o: asem.h

coverage: clean
	$(MAKE) COVERAGE=$(COV)

gcov:
	gcov *.c

test: $(PROGS)
	@for i in test-*.sh ; do echo $$i ; sh timeout.sh $$i || exit 1 ; done

couverture-et-tests: clean coverage test gcov

ctags:	
	ctags *.[ch]

rapport.pdf:	rapport.tex
	pdflatex rapport
	pdflatex rapport

# génère un fichier devoir.tgz contenant les fichiers à déposer sur Moodle
devoir.tgz: $(DEVOIR)
	tar czvf devoir.tgz $(DEVOIR)

clean:
	rm -f *.o $(PROGS)
	rm -f *.gc*
	rm -f *.log
	rm -f *.aux *.pdf
	rm -f tags core moodle.tgz
	rm -rf moodle
	rm -f devoir.tgz

