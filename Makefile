# compilateur
CC := g++
# options de compilation
CFLAGS := -std=c++98 -Wall -Wextra -pedantic -ggdb -Wno-unused-variable -Wno-unused-parameter

CLIENT := client
SERVEUR := serveur

LIBCBOR := -lcbor

# cibles par défaut
all : $(CLIENT) $(SERVEUR)


# règles de compilation pour le client
$(CLIENT) : client.o main_client.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBCBOR)

# règles de compilation pour le serveur
$(SERVEUR) : serveur.o main_serveur.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBCBOR)

# règle générique pour la compilation des fichiers .o
%.o : %.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

# nettoyage des fichiers temporaires et des exécutables
clean :
	rm -f *.o  $(CLIENT) $(SERVEUR)

# lancement des différentes cibles

run_client : $(CLIENT)
	./$(CLIENT)

run_serveur : $(SERVEUR)
	./$(SERVEUR)

# vérification de la mémoire avec Valgrind
VFLAGS := --leak-check=full


memoire_client : $(CLIENT)
	valgrind $(VFLAGS) ./$(CLIENT)

memoire_serveur : $(SERVEUR)
	valgrind $(VFLAGS) ./$(SERVEUR)
