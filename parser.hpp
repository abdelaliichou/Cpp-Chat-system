#ifndef PARSER_HPP
#define PARSER_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <utility>
#include <iomanip>
#include <map> 
#include "trajictoire.hpp"

class Parser{
	Parser() {}
};


// Function to read column positions from the header
void readColumnPositions(std::ifstream& inputFile, std::map<std::string, int>* columnPositions) {
    columnPositions->clear(); 
    std::string header;
    if (std::getline(inputFile, header)) {
        std::stringstream headerStream(header);
        std::string columnName;
        int columnCount = 0;
        while (std::getline(headerStream, columnName, ';')) {
            (*columnPositions)[columnName] = columnCount++;
        }
    }
}


std::vector<Trajectoire>& extractTrajectoriesFromFile(const std::string& filename, const std::map<std::string, int>& columnPositions, std::vector<Trajectoire>& trajectories) {
    std::ifstream inputFile(filename.c_str());
    if (!inputFile.is_open()) {
        std::cerr << "Error opening input file: " << filename << std::endl;
        return trajectories; // Return the input vector reference without modifying it
    }

    std::string line;
    double xCoord, yCoord;
    int currentTrajectoryID = -1; // Initialize current trajectory ID
    Trajectoire currentTrajectory; // Initialize current trajectory object

    while (std::getline(inputFile, line)) {
        std::stringstream ss(line);
        std::string token;
        int columnIndex = 0;

        while (std::getline(ss, token, ';')) {
            if (columnIndex == columnPositions.at("track_id")) {
                int trajectoryID = std::atoi(token.c_str());
                if (trajectoryID != currentTrajectoryID) {
                    // If trajectory ID changes, push the current trajectory to the vector
                    if (!currentTrajectory.obtenirPoints().empty()) {
                        trajectories.push_back(currentTrajectory);
                        currentTrajectory = Trajectoire(); // Reset current trajectory
                    }
                    // Update current trajectory ID
                    currentTrajectoryID = trajectoryID;
                }
            } else if (columnIndex == columnPositions.at("x")) {
                xCoord = std::atof(token.c_str());
            } else if (columnIndex == columnPositions.at("y")) {
                yCoord = std::atof(token.c_str());
            }
            columnIndex++;
        }

        // Add point to the current trajectory
        currentTrajectory.ajouterPoint(Point(xCoord, yCoord));
    }

    // Push the last trajectory to the vector
    if (!currentTrajectory.obtenirPoints().empty()) {
    	trajectories.erase(trajectories.begin());
        trajectories.push_back(currentTrajectory);
    }

    inputFile.close();
    return trajectories;
}

#endif 
