# Définition du compilateur
CC = gcc

# Options de compilation
CFLAGS = -Wall -Wextra -O3

# Nom de l'exécutable final
EXEC = water_processor

# Liste des fichiers objets (AJOUT de leaks.o ici)
OBJ = main.o avl.o leaks.o

# Règle principale
all: $(EXEC)

# Création de l'exécutable à partir des objets
$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(EXEC)

# Compilation du fichier main.c (dépend de avl.h)
main.o: main.c avl.h
	$(CC) $(CFLAGS) -c main.c

# Compilation du fichier avl.c (dépend de avl.h)
avl.o: avl.c avl.h
	$(CC) $(CFLAGS) -c avl.c

# Compilation du fichier leaks.c (dépend de avl.h)
leaks.o: leaks.c avl.h
	$(CC) $(CFLAGS) -c leaks.c

# Règle pour nettoyer les fichiers temporaires et les graphiques
clean:
	rm -f *.o $(EXEC) resultat_*.dat top10.tmp bot50.tmp

# Règle pour tout recompiler
rebuild: clean all
