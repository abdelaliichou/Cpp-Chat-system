#ifndef SERVEUR_HPP
#define SERVEUR_HPP

#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <cbor.h>
#include "trajictoire.hpp"
#include "ensembleProfils.hpp"
#include <string.h>

class ServeurSocket {
private:
    int socketServeur;
    int socketClient;
    struct sockaddr_in adresseServeur;
    struct sockaddr_in adresseClient;

public:
    ServeurSocket(int port);
    void ecouterClients();
    void traiterConnexionAvecClient();
    void fermerConnexion();
    void extractTrajectoires(const Trajectoire& trajectoire1);
    Trajectoire dechiffrerTrajectoire(const cbor_item_t* trajItem);
    void extractProfils(const std::vector<int>& profilReference, double seuil);
    void dataType(const cbor_item_t* cborMessage,char& choix);
    std::vector<int> dechiffrerEnsemble(const cbor_item_t* cborMessage);
};

#endif // SERVEUR_HPP
