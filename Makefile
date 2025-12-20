# Définition du compilateur
CC = gcc

# Options de compilation :
# -O3 est crucial ici pour la rapidité sur les gros fichiers CSV
CFLAGS = -Wall -Wextra -O3

# Nom de l'exécutable final
EXEC = water_processor

# AJOUT de leaks.o ici pour que le compilateur l'intègre
OBJ = main.o avl.o leaks.o

# Règle principale
all: $(EXEC)

# Création de l'exécutable
$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(EXEC)

# Compilation du fichier main.c (dépend de avl.h)
main.o: main.c avl.h
	$(CC) $(CFLAGS) -c main.c

# Compilation du fichier avl.c
avl.o: avl.c avl.h
	$(CC) $(CFLAGS) -c avl.c

# AJOUT de la compilation pour leaks.c
leaks.o: leaks.c avl.h
	$(CC) $(CFLAGS) -c leaks.c

# Nettoyage
clean:
	rm -f *.o $(EXEC)

# Tout recompiler
rebuild: clean all
