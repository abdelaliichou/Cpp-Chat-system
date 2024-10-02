#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <cstring>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <ctime>
#include <arpa/inet.h>
#include <cbor.h>
#include "trajictoire.hpp" 
#include "ensembleProfils.hpp"

class ClientSocket {
private:
    int socketClient;
    struct sockaddr_in adresseServeur;

public:
    ClientSocket(const char* adresseServeurIP, int port);
    void envoyerDonnees(cbor_item_t* message);
    void fermerConnexion();
};
#endif // CLIENT_HPP
