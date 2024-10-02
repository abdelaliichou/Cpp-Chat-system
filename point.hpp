#ifndef POINT_HPP
#define POINT_HPP
#include <cbor.h>
#include <iostream>

class Point {
private:
    double x;
    double y;

public:
    // Constructeurs
    Point() : x(0.0), y(0.0) {}
    Point(double xCoord, double yCoord) : x(xCoord), y(yCoord) {}

    // M�thodes pour obtenir les coordonn�es
    double getX() const { return x; }
    double getY() const { return y; }

    // M�thodes pour d�finir les coordonn�es
    void setX(double xCoord) { x = xCoord; }
    void setY(double yCoord) { y = yCoord; }

    // M�thode pour afficher le point
    void afficher() const {
        std::cout << "(" <<  std::fixed << x << ", " << std::fixed<< y << ")";
    }

    //Converting the point to a cbor item
    cbor_item_t* to_cbor() const {
        cbor_item_t* point = cbor_new_definite_array(2);
        cbor_array_push(point, cbor_build_float8(x));
        cbor_array_push(point, cbor_build_float8(y));
        return point;
    }
};

#endif

