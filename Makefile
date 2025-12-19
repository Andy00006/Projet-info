# Définition du compilateur
CC = gcc

# Options de compilation :
# -Wall -Wextra : affiche tous les avertissements pour un code propre
# -O3 : optimisation maximale pour la vitesse de traitement
CFLAGS = -Wall -Wextra -O3

# Nom de l'exécutable final
EXEC = water_processor

# Liste des fichiers objets (les fichiers .c compilés en .o)
OBJ = main.o avl.o

# Règle principale (appelée par défaut via la commande 'make')
all: $(EXEC)

# Création de l'exécutable à partir des objets
$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(EXEC)

# Compilation du fichier main.c
main.o: main.c avl.h
	$(CC) $(CFLAGS) -c main.c

# Compilation du fichier avl.c
avl.o: avl.c avl.h
	$(CC) $(CFLAGS) -c avl.c

# Règle pour nettoyer les fichiers temporaires
clean:
	rm -f *.o $(EXEC)

# Règle pour tout recompiler
rebuild: clean all

