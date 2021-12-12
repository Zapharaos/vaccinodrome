# Vaccinodrome

Implementation of a vaccinodrome with random doctors and patients. It was realized within the framework of a project in the fifth semester of my bachelor's degree (2021).

## Tests

To start the tests (already provided) :
```
make test
```
Check the makefile to see all the other commands that are enabled.

## Run

ouvrir.c : opens the vaccinodrome with n seats, m doctors and t the duration of each vaccination.
```
./ouvrir <n> <m> <t>
```

fermer.c : closes the vaccinodrome after everyone has left and cleans it.
```
./fermer
```

nettoyer.c : cleans the vaccinodrome.
```
./nettoyer
```

medecin.c : starts one doctor.
```
./medecin
```

patient.c : starts one patient whose name is given.
```
./patient <name>
```

## Enjoy
