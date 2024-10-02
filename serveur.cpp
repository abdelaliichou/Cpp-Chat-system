#include "serveur.hpp"
#include "parser.hpp"

ServeurSocket::ServeurSocket(int port) {
    // Initialisation du socket serveur
    socketServeur = socket(AF_INET, SOCK_STREAM, 0);
    if (socketServeur == -1) {
        std::cerr << "Erreur lors de la création du socket serveur." << std::endl;
        _exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse du serveur
    adresseServeur.sin_family = AF_INET;
    adresseServeur.sin_addr.s_addr = INADDR_ANY;
    adresseServeur.sin_port = htons(port);

    // Liaison du socket à l'adresse du serveur
    if (bind(socketServeur, (struct sockaddr*)&adresseServeur, sizeof(adresseServeur)) == -1) {
        std::cerr << "Erreur lors de la liaison du socket serveur." << std::endl;
        _exit(EXIT_FAILURE);
    }
}

void ServeurSocket::ecouterClients() {
    // Mise en attente de connexions entrantes
    listen(socketServeur, 5);
    std::cout << "Serveur en attente de connexions..." << std::endl;

    while (true) {
        // Acceptation d'une connexion entrante
        socklen_t tailleAdresseClient = sizeof(adresseClient);
        socketClient = accept(socketServeur, (struct sockaddr*)&adresseClient, &tailleAdresseClient);
        if (socketClient == -1) {
            std::cerr << "Erreur lors de l'acceptation de la connexion client." << std::endl;
            continue; // Continue à attendre une nouvelle connexion
        }

        std::cout << "Nouveau client connecté !" << std::endl;

        // Traitement de la connexion avec le client
        traiterConnexionAvecClient();

        // Fermeture de la connexion avec le client actuel
        close(socketClient);
    }
}

void ServeurSocket::traiterConnexionAvecClient() {
    // Exemple : réception et traitement des données du client
    unsigned char buffer[1024];
    ssize_t octetsLus = recv(socketClient, buffer, sizeof(buffer), 0);
    if (octetsLus > 0) {
        // Charger les données reçues en tant qu'élément CBOR
        cbor_item_t* messageRecu;
        struct cbor_load_result result;
        messageRecu = cbor_load(buffer, octetsLus, &result);
        if (messageRecu != NULL) {
            // Afficher le contenu du message CBOR
            cbor_describe(messageRecu, stdout);
            std::cout << std::endl;

            char choix;

            // decide if trajectoire or ensemble
            dataType(messageRecu, choix);

            std::cout << "Your input was ===>  " << choix << std::endl;

            if ((choix == 'T') || (choix == 't')) {

                // Dechifrement du message CBOR en apellant la fonction des trajectiores 
                Trajectoire path = dechiffrerTrajectoire(messageRecu);
                
                std::cout << "\nEtude de similarite des Trajectoire en cours \n" << std::endl;

                // afficher le trajectoires
                std::cout << "Le Trajectoire passe en parametre : " << std::endl;
                path.afficher();

                // Traitement des trajectoires et trouver le plus similaire pour lui
                extractTrajectoires(path);

            } else if ( (choix == 'E') || (choix == 'e') ){

                // Dechifrement du message CBOR en apellant la fonction des ensembles 
                std::vector<int> path = dechiffrerEnsemble(messageRecu);
                
                std::cout << "Etude de similarite des Ensembles en cours  " << std::endl;

                // Traitement des ensembles et trouver le plus similaire pour lui
                extractProfils(path, 0.5);

            } else {
                std::cout << "You didn't chouse well the type of etude you want !  " << std::endl;
                return; 
            }

            
            // Libérer la mémoire allouée pour le message CBOR
            cbor_decref(&messageRecu);
        } else {
            std::cerr << "Erreur lors du chargement des données CBOR." << std::endl;
        }
    } else if (octetsLus == 0) {
        std::cout << "Client déconnecté." << std::endl;
    } else {
        std::cerr << "Erreur lors de la réception des données du client." << std::endl;
    }
}


// function to know what study we do, the ensemble one or the trajectoire one 
void ServeurSocket::dataType(const cbor_item_t* cborMessage,char& choix){
    // Extracting the type string and copying it to the choix variable
    cbor_pair pair2 = cbor_map_handle(cborMessage)[1];
    cbor_item_t* type = pair2.value;

    // Allocate memory for the string buffer (+1 for the null terminator)
    char* typeStr = new char[2];

    // Copy the string data
    memcpy(typeStr, cbor_string_handle(type), 2);
    typeStr[2] = '\0';

    choix = typeStr[0];

    // free the allocated memory when you're done
    delete[] typeStr;
}


// Function to decode Trajectoire from CBOR response
Trajectoire ServeurSocket::dechiffrerTrajectoire(const cbor_item_t* cborMessage) {

    Trajectoire trajectoire;
    cbor_pair pair = cbor_map_handle(cborMessage)[0];

    // Extracting the trajectoire
    cbor_item_t* pointsArray = pair.value;
    size_t arraySize = cbor_array_size(pointsArray);
    for (size_t j = 0; j < arraySize; ++j) {
        cbor_item_t* pointArray = cbor_array_handle(pointsArray)[j];
        if (cbor_isa_array(pointArray) &&
            cbor_array_size(pointArray) == 2 &&
            cbor_is_float(cbor_array_handle(pointArray)[0]) &&
            cbor_is_float(cbor_array_handle(pointArray)[1])) {
            double x = cbor_float_get_float8(cbor_array_handle(pointArray)[0]);
            double y = cbor_float_get_float8(cbor_array_handle(pointArray)[1]);

            trajectoire.ajouterPoint(Point(x, y));
        }
    }

    return trajectoire;
}


// Function to decode Ensemble from CBOR response
std::vector<int> ServeurSocket::dechiffrerEnsemble(const cbor_item_t* cborMessage) {

    std::vector<int> vec;

    size_t arraySize = cbor_array_size(cborMessage);
    for (size_t i = 0; i < arraySize; ++i) {
        const cbor_item_t* item = cbor_array_handle(cborMessage)[i];
        if (!cbor_isa_uint(item)) {
            // Gérer le cas où l'élément CBOR n'est pas un entier
            continue;
        }
        vec.push_back(cbor_get_uint8(item)); // Ajouter l'entier au vecteur
    }
    return vec;
}


void ServeurSocket::extractProfils(const std::vector<int>& profilReference, double seuil){
    // Création d'un ensemble de profils
    EnsembleProfils ensembleProfils;

    // Chargement des profils depuis le fichier "netflix"
    ensembleProfils.chargerDepuisFichier("netflix");

    // Recherche de profils similaires avec un seuil de 0.5
    std::vector<std::vector<int> > profilsSimilaires = ensembleProfils.trouverProfilsSimilaires(profilReference, seuil);

    // Affichage des profils similaires
    std::cout << "Profils similaires au profil de référence : " << std::endl;
    for (size_t i = 0; i < profilsSimilaires.size(); ++i) {
        std::cout << "Profil " << i + 1 << ": ";
        for (size_t j = 0; j < profilsSimilaires[i].size(); ++j) {
            std::cout << profilsSimilaires[i][j] << " ";
        }
        std::cout << std::endl;
    }
}



void ServeurSocket::extractTrajectoires(const Trajectoire& path) {
    // Open the input CSV file
    std::ifstream inputFile("pigeons.csv");
        
    
    // Check if the file opened successfully
    if (!inputFile.is_open()) {
        std::cerr << "Error opening input file\n";
        return;
    }
    
    // Read column positions from the header
	std::map<std::string, int> columnPositions;
	readColumnPositions(inputFile, &columnPositions);


	// Close the input file
	inputFile.close();

    // Define the filename and column positions
    std::string filename = "pigeons.csv"; 

     std::vector<Trajectoire> trajectories;
    // Call the function to populate trajectories directly
    extractTrajectoriesFromFile(filename, columnPositions, trajectories);
       
    
	//Soit une trajectoire t du client, quelle est la trajectoire la plus proche dans le jeu de donn�ees de r�ef�erence   
  	std::pair<double, Trajectoire> result = Trajectoire::findSmallestDiscreteFrechetDistance(path, trajectories);
	double smallestDistance = result.first;
	Trajectoire bestTrajectoire = result.second;
	
	std::cout << "The smallest distance is " << smallestDistance <<std::endl;

    double threshold = smallestDistance+100.0; // Threshold value provided by the client


 	//Soit une trajectoire t du client, quel est le nombre de trajectoires similaires dans le jeu de donn�ees de r�ef�erence pour un seuil ? d�efini par le client
	size_t count = Trajectoire::countTrajectoriesWithinDistance(path, trajectories, threshold);
	std::cout << "Number of trajectories within distance " << threshold << ": " << count << std::endl;
	
	
	//Quelles sont les trajectoires similaires dans le jeu de donn�ees de r�ef�erence pour ce seuil ?
	const std::vector<Trajectoire>* similarTrajectories = Trajectoire::findTrajectoriesWithinDistance(path, trajectories, threshold);
	Trajectoire::displaySimilarTrajectories(similarTrajectories);
}

void ServeurSocket::fermerConnexion() {
    // Fermeture des sockets
    close(socketClient);
    close(socketServeur);
}
