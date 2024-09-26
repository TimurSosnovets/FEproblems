#include "../../headers/mechanical/2dPoint.hpp"

Point::Point(double x, double y) : x(x), y(y) { }

double Point::distance_to(const Point& p) const {
    return sqrt(pow(x-p.x,2.0f)+pow(y-p.y,2.0f));
}

double Point::coord_x() const {
    return x;
} 

double Point::coord_y() const {
    return y;
}

void Point::get_info () {
    std::cout << "Point: " << "(" << coord_x() << ", " << coord_y() << ").";
}


