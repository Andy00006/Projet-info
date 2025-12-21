# Définition du compilateur et des options
CC = gcc
CFLAGS = -Wall -Wextra -O3

# Nom de l'exécutable final
EXEC = water_processor

# Liste des fichiers objets
OBJ = main.o avl.o leaks.o

# PREMIÈRE CIBLE : Compile le projet (appelée par défaut via 'make')
all: $(EXEC)

# Règle pour créer l'exécutable
$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(EXEC)

# Compilation des modules individuels
main.o: main.c avl.h
	$(CC) $(CFLAGS) -c main.c

avl.o: avl.c avl.h
	$(CC) $(CFLAGS) -c avl.c

leaks.o: leaks.c avl.h
	$(CC) $(CFLAGS) -c leaks.c

# CIBLE CLEAN : Efface tous les fichiers générés
clean:
	rm -f *.o $(EXEC) resultat_*.dat rendement_historique.dat top10.tmp bot50.tmp *.png
	@echo "Nettoyage des fichiers binaires, données et graphiques terminé."

# Règle pour forcer une recompilation propre
rebuild: clean all
