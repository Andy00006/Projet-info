#Paramètres de compilation
CC = gcc
CFLAGS = -Wall -Wextra -O3
EXEC = water_processor
OBJ = main.o avl.o leaks.o

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(EXEC)

#Compilation des modules
main.o: main.c avl.h
	$(CC) $(CFLAGS) -c main.c

avl.o: avl.c avl.h
	$(CC) $(CFLAGS) -c avl.c

leaks.o: leaks.c avl.h
	$(CC) $(CFLAGS) -c leaks.c

#Nettoyage du projet
clean:
	rm -f *.o $(EXEC) resultat_*.dat rendement_historique.dat top10.tmp bot50.tmp *.png
	@echo "Nettoyage des fichiers binaires, données et graphiques terminé."

rebuild: clean all
