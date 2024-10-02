#include "client.hpp"

ClientSocket::ClientSocket(const char* adresseServeurIP, int port) {
    // Création du socket client
    socketClient = socket(AF_INET, SOCK_STREAM, 0);
    if (socketClient == -1) {
        std::cerr << "Erreur lors de la création du socket client." << std::endl;
        _exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse du serveur
    adresseServeur.sin_family = AF_INET;
    adresseServeur.sin_addr.s_addr = inet_addr(adresseServeurIP);
    adresseServeur.sin_port = htons(port);

    // Connexion au serveur
    if (connect(socketClient, (struct sockaddr*)&adresseServeur, sizeof(adresseServeur)) == -1) {
        std::cerr << "Erreur lors de la connexion au serveur." << std::endl;
        _exit(EXIT_FAILURE);
    }
}

void ClientSocket::envoyerDonnees(cbor_item_t* message) {
    // Sérialisation du message CBOR
    unsigned char *buffer;
    size_t buffer_size;
    buffer_size = cbor_serialize_alloc(message, &buffer, &buffer_size);

    // Envoi du message sérialisé au serveur
    send(socketClient, buffer, buffer_size, 0);

    // Libération de la mémoire allouée pour le tampon
    free(buffer);
}

void ClientSocket::fermerConnexion() {
    // Fermeture du socket client
    close(socketClient);
}