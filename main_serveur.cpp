#include "serveur.hpp"

int main() {
    ServeurSocket serveur(8080);
    serveur.ecouterClients();
    //serveur.recevoirDonnees();
    serveur.fermerConnexion();
    return 0;
}
