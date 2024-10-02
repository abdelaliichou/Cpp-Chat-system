#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cmath>

// Structure to represent a point in a trajectory
struct Point {
    double x;
    double y;
};

// Structure to represent a trajectory
struct Trajectory {
    std::vector<Point> points;
};

// Function to calculate the Euclidean distance between two points
double distance(const Point& p1, const Point& p2) {
    return std::sqrt(std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2));
}

// Function to find the closest trajectory to a given trajectory
Trajectory findClosestTrajectory(const Trajectory& query, const std::vector<Trajectory>& trajectories) {
    double minDistance = std::numeric_limits<double>::max();
    Trajectory closestTrajectory;

    for (const auto& trajectory : trajectories) {
        double totalDistance = 0.0;
        for (size_t i = 0; i < trajectory.points.size(); ++i) {
            totalDistance += distance(query.points[i], trajectory.points[i]);
        }
        if (totalDistance < minDistance) {
            minDistance = totalDistance;
            closestTrajectory = trajectory;
        }
    }

    return closestTrajectory;
}

// Function to read trajectories from a CSV file
std::vector<Trajectory> readTrajectoriesFromCSV(const std::string& filename) {
    std::vector<Trajectory> trajectories;
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
        Trajectory trajectory;
        std::istringstream iss(line);
        std::string x, y;

        while (std::getline(iss, x, ',') && std::getline(iss, y, ',')) {
            Point point;
            point.x = std::stod(x);
            point.y = std::stod(y);
            trajectory.points.push_back(point);
        }

        trajectories.push_back(trajectory);
    }

    return trajectories;
}

int main() {
    // Read trajectories from CSV file
    std::vector<Trajectory> trajectories = readTrajectoriesFromCSV("trajectories.csv");

    // Example query trajectory
    Trajectory queryTrajectory;
    // Populate queryTrajectory with the client's request trajectory

    // Find the closest trajectory
    Trajectory closestTrajectory = findClosestTrajectory(queryTrajectory, trajectories);

    // Send closestTrajectory back to the client

    return 0;
}

