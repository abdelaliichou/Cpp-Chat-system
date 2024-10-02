#ifndef TRAJICTOIRE_HPP
#define TRAJICTOIRE_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <utility>
#include <iomanip>
#include <map> 
#include "point.hpp" 

class Trajectoire {
private:
    std::vector<Point> points;

public:
    // Constructeur prenant un vecteur de points comme param�tre
    Trajectoire(const std::vector<Point>& points) : points(points) {}
    Trajectoire() {}

    // Cbor converting method
    cbor_item_t* to_cbor() const {
        cbor_item_t* traj = cbor_new_definite_array(points.size());
        for (std::vector<Point>::const_iterator it = points.begin(); it != points.end(); ++it) {
        // for (const Point& point : points) {
            const Point& point = *it;
            cbor_array_push(traj, point.to_cbor());
        }
        return traj;
    }

    // M�thode pour ajouter un point � la trajectoire
    void ajouterPoint(const Point& point) {
        points.push_back(point);
    }

    // M�thode pour obtenir les points de la trajectoire
    const std::vector<Point>& obtenirPoints() const {
        return points;
    }

    // M�thode pour afficher la trajectoire
    void afficher() const {
        std::cout << "Trajectoire :";
        std::vector<Point>::const_iterator it;
        for (it = points.begin(); it != points.end(); ++it) {
            it->afficher();
        }
        std::cout << "\n\n\n\n\n" << std::endl;
    }    
    
    //Quelles sont les trajectoires similaires dans le jeu de donn�ees de r�ef�erence pour ce seuil ?
    static const std::vector<Trajectoire>* findTrajectoriesWithinDistance(
    const Trajectoire& givenTrajectory,
    const std::vector<Trajectoire>& trajectories,
    double threshold
) {
    std::vector<Trajectoire>* similarTrajectories = new std::vector<Trajectoire>();

    for (size_t i = 0; i < trajectories.size(); ++i) {
        double distance = givenTrajectory.computeDiscreteFrechetDistance(trajectories[i]);
        if (distance < threshold) {
            similarTrajectories->push_back(trajectories[i]);
        }
    }

    if (similarTrajectories->empty()) {
        delete similarTrajectories; // Clean up if no similar trajectories found
        return similarTrajectories;
    }

    return similarTrajectories;
}

    
    
    //Soit une trajectoire t du client, quel est le nombre de trajectoires similaires dans le jeu de donn�ees de r�ef�erence pour un seuil ? d�efini par le client
    static size_t countTrajectoriesWithinDistance(
        const Trajectoire& givenTrajectory,
        const std::vector<Trajectoire>& trajectories,
        double threshold
    ) {
        size_t count = 0;
        for (size_t i = 0; i < trajectories.size(); ++i) {
            double distance = givenTrajectory.computeDiscreteFrechetDistance(trajectories[i]);
            if (distance < threshold) {
                ++count;
            }
        }
        return count;
    }
    
    
    
    
//Soit une trajectoire t du client, quelle est la trajectoire la plus proche dans le jeu de donn�ees de r�ef�erence
static std::pair<double, Trajectoire> findSmallestDiscreteFrechetDistance(
    const Trajectoire& trajectoire1,
    const std::vector<Trajectoire>& trajectories,
    size_t startIndex = 0 // Default starting index
) {
    // Initialize best distance and trajectory
    double bestDistance = std::numeric_limits<double>::max();
    Trajectoire bestTrajectory;

    // Iterate over trajectories starting from the specified index
    for (size_t i = startIndex; i < trajectories.size(); ++i) {
        double distance = trajectoire1.computeDiscreteFrechetDistance(trajectories[i]);
        if (distance < bestDistance) {
            bestDistance = distance;
            bestTrajectory = trajectories[i];
        }
    }

    return std::make_pair(bestDistance, bestTrajectory);
}


    // Fonction pour calculer la distance de Fr�chet discr�te entre cette trajectoire et une autre trajectoire
    double computeDiscreteFrechetDistance(const Trajectoire& other) const {
        std::vector<std::vector<double> > partialDistances(points.size(), std::vector<double>(other.points.size(), -1.0));
        return std::sqrt(computeRecursive(points, other.points, partialDistances, points.size() - 1, other.points.size() - 1));
    }
    
    
    
    
      
   static void displaySimilarTrajectories(const std::vector<Trajectoire>* similarTrajectories) {
    if (similarTrajectories) {
        // Output the count of similar trajectories
        std::cout << "Number of similar trajectories found: " << similarTrajectories->size() << std::endl;

        // Output each similar trajectory
        for (size_t i = 0; i < similarTrajectories->size(); ++i) {
            std::cout << "Similar Trajectory " << i+1 << ":" << std::endl;
            // Access the trajectory using the pointer to the vector
            (*similarTrajectories)[i].afficher(); // Assuming afficher() prints the trajectory
        }

        // Clean up the dynamically allocated vector
        delete similarTrajectories;
    } else {
        std::cout << "No similar trajectories found." << std::endl;
    }
}


private:
    double squaredDistance(const Point& p1, const Point& p2) const {
        double dx = p1.getX() - p2.getX();
        double dy = p1.getY() - p2.getY();
        return dx * dx + dy * dy;
    }

    // Fonction r�cursive pour calculer la distance de Fr�chet discr�te
    double computeRecursive(const std::vector<Point>& traj1, const std::vector<Point>& traj2, std::vector<std::vector<double> >& partialDistances, int i, int j) const {
        if (partialDistances[i][j] > -1.0) {
            return partialDistances[i][j];
        } else if (i == 0 && j == 0) {
            partialDistances[i][j] = squaredDistance(traj1[0], traj2[0]);
        } else if (i > 0 && j == 0) {
            partialDistances[i][j] = std::max(computeRecursive(traj1, traj2, partialDistances, i - 1, 0), squaredDistance(traj1[i], traj2[0]));
        } else if (i == 0 && j > 0) {
            partialDistances[i][j] = std::max(computeRecursive(traj1, traj2, partialDistances, 0, j - 1), squaredDistance(traj1[0], traj2[j]));
        } else if (i > 0 && j > 0) {
            double minPrev = std::min(computeRecursive(traj1, traj2, partialDistances, i - 1, j), computeRecursive(traj1, traj2, partialDistances, i, j - 1));
            partialDistances[i][j] = std::max(std::min(minPrev, computeRecursive(traj1, traj2, partialDistances, i - 1, j - 1)), squaredDistance(traj1[i], traj2[j]));
        } else {
            partialDistances[i][j] = INFINITY;
        }
        return partialDistances[i][j];
    }

};
#endif

