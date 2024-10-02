#include "client.hpp"



std::string UserInteraction(){
    std::string answer1;

    // Prompt the user if he want to deal with trajectoire or with ensembles
    std::cout << "For using the Trajectories enter <T>, for the Ensembles enter <E> \n";
    std::getline(std::cin, answer1);


   
    if((answer1 == "T") || (answer1 == "t")){
        std::cout << "Etude de similarite des Trajectoire en cours " << std::endl;
    } else if ( (answer1 == "E") || (answer1 == "e") ){
        std::cout << "Etude de similarite des Ensembles en cours  " << std::endl;
    } else {
        return "FALSE";
    }

    return answer1;
}

cbor_item_t* vector_to_cbor(const std::vector<int>& vec) {
    cbor_item_t* array = cbor_new_definite_array(vec.size());
    for (size_t i = 0; i < vec.size(); ++i) {
        cbor_array_set(array, i, cbor_build_uint8(vec[i]));
    }
        
    return array;
}


int main() {

    ClientSocket client("127.0.0.1", 8080);

    // Cr�ation de quelques points pour la premi�re trajectoire pour les passes en parameteres
    std::vector<Point> traj1;
    traj1.push_back(Point(0.3, 1.6));
    traj1.push_back(Point(3.2, 3.0));
    traj1.push_back(Point(3.8, 1.8));
    traj1.push_back(Point(5.2, 3.1));
    traj1.push_back(Point(6.5, 2.8));
    traj1.push_back(Point(7.0, 0.8));
    traj1.push_back(Point(8.9, 0.6));
    std::vector<Point> traj2;
    traj2.push_back(Point(0.2, 2.0));
    traj2.push_back(Point(1.5, 2.8));
    traj2.push_back(Point(2.3, 1.6));
    traj2.push_back(Point(2.9, 1.8));
    traj2.push_back(Point(4.1, 3.1));
    traj2.push_back(Point(5.6, 2.9));
    traj2.push_back(Point(7.2, 1.3));
    traj2.push_back(Point(8.2, 1.1));
    std::vector<Point> traj3;
    traj3.push_back(Point(611933.31323,5728949.36779));
    traj3.push_back(Point(615968.25837,5733390.16179));
    traj3.push_back(Point(616126.24786,5733609.8373));
    traj3.push_back(Point(616298.80399,5733868.31896));
    traj3.push_back(Point(616561.13317,5737384.01779));
    traj3.push_back(Point(614214.07688,5732207.81932));
    traj3.push_back(Point(614229.40762,5732234.87894));
    traj3.push_back(Point(614694.59486,5732538.11658));

    // Cr�ation des trajectoires
    Trajectoire trajectoire1(traj1);
    Trajectoire trajectoire2(traj2);
    Trajectoire trajectoire3(traj3);

    // creation d'un ensemble de prfile
    // Profil de référence pour lequel on souhaite obtenir des recommandations
    std::vector<int> profilReference;
    profilReference.push_back(1);
    profilReference.push_back(3);
    profilReference.push_back(17);
    profilReference.push_back(30);

    // client entre le type de l'etude de similarity qu'il veux 
    std::string etud = UserInteraction();

    // means that the user entered correctly the type
    if (etud != "FALSE"){

        // Defining the pairs to the info thta we will send in the CBOR message
        struct cbor_pair pair1, pair2;

        // Defining the CBOR message structur with trajectoires 
        cbor_item_t* message = cbor_new_definite_map(2);

        // preparing the tyoe of etude as a CBOR message
        cbor_item_t* type = cbor_build_string(etud.c_str());


        if ((etud == "E") || (etud == "e")){   

            pair1.key = cbor_move(cbor_build_string("Ensemble1"));
            cbor_item_t* ensemble = vector_to_cbor(profilReference);
            pair1.value = ensemble;
            cbor_map_add(message, pair1);

        } else if ((etud == "T") || (etud == "t")){
            pair1.key = cbor_move(cbor_build_string("Trajectoire1"));
            pair1.value = trajectoire3.to_cbor();
            cbor_map_add(message, pair1);

        }
        
        
        // Adding the type to the CBOR message
        pair2.key = cbor_move(cbor_build_string("Type d'etude"));
        pair2.value = type;
        cbor_map_add(message, pair2);

    
        // Envoi du message CBOR au serveur
        client.envoyerDonnees(message);


        // Libération de la mémoire allouée pour le message CBOR
        cbor_decref(&message);

        return 0;

    } else {
        std::cout << "Sorry, you entered a wrong input !" << std::endl;
        return 0;
    }
}

 /*
[CBOR_TYPE_MAP] Definite, size: 2: Cela indique que le message reçu du client est un objet de type "map" (une sorte de tableau associatif ou de dictionnaire) avec une taille fixe de 2 éléments.

[CBOR_TYPE_STRING] Definite, length 4B, 4 codepoints: Cela indique que le premier élément de la map est une chaîne de caractères (string) avec une longueur de 4 octets et 4 caractères.

    key1: C'est la clé associée à la première valeur dans la map.

[CBOR_TYPE_FLOAT_CTRL] Width: 8B, value: 3,140000: Cela indique que la première valeur dans la map est un nombre flottant de 8 octets (double précision) avec la valeur 3.14.

[CBOR_TYPE_STRING] Definite, length 4B, 4 codepoints: Cela indique que le deuxième élément de la map est une chaîne de caractères (string) avec une longueur de 4 octets et 4 caractères.

    key2: C'est la clé associée à la deuxième valeur dans la map.

[CBOR_TYPE_STRING] Definite, length 6B, 6 codepoints: Cela indique que la deuxième valeur dans la map est une chaîne de caractères (string) avec une longueur de 6 octets et 6 caractères.

    value2: C'est la valeur associée à la clé "key2" dans la map.*/

