#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <set>


class EnsembleProfils {
private:
    std::vector<std::vector<int> > profils;

public:
    EnsembleProfils() {}

    // Méthode pour charger les profils depuis le fichier netflix
    void chargerDepuisFichier(const std::string& nomFichier) {
        std::ifstream fichier(nomFichier.c_str());
        if (!fichier.is_open()) {
            std::cerr << "Erreur lors de l'ouverture du fichier " << nomFichier << std::endl;
            return;
        }

        std::string ligne;
        while (std::getline(fichier, ligne)) {
            std::vector<int> profil;
            std::istringstream iss(ligne);
            int motCle;
            while (iss >> motCle) {
                profil.push_back(motCle);
            }
            profils.push_back(profil);
        }

        fichier.close();
    }

    // Méthode pour trouver les profils similaires à un profil donné avec une certaine distance seuil
    std::vector<std::vector<int> > trouverProfilsSimilaires(const std::vector<int>& profilReference, double seuil) const {
        std::vector<std::vector<int> > profilsSimilaires;

        // Comparaison avec tous les profils dans la base de données
        for (size_t i = 0; i < profils.size(); ++i) {
            double distance = calculerDistanceJaccard(profilReference, profils[i]);
            if (distance <= seuil) {
                profilsSimilaires.push_back(profils[i]);
            }
        }

        return profilsSimilaires;
    }

private:
    // Méthode pour calculer la distance de Jaccard entre deux profils
    double calculerDistanceJaccard(const std::vector<int>& profil1, const std::vector<int>& profil2) const {
        std::set<int> ensembleProfil1(profil1.begin(), profil1.end());
        std::set<int> ensembleProfil2(profil2.begin(), profil2.end());

        size_t intersection = 0;
        size_t unionSetSize = ensembleProfil1.size() + ensembleProfil2.size();

        for (std::set<int>::iterator it = ensembleProfil1.begin(); it != ensembleProfil1.end(); ++it) {
            if (ensembleProfil2.count(*it)) {
                intersection++;
            }
        }

        double indice = static_cast<double>(intersection) / (unionSetSize - intersection);

        return 1.0 - indice; // La distance de Jaccard est 1 - indice
    }
};
