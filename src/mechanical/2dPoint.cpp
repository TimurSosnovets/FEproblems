#include "../../headers/mechanical/2dPoint.hpp"

//Функции сравнения
bool are_close_enough(double a, double b, double tol) {
    return (abs((a-b))<=tol);
};

bool is_close_to_zero(double a, double tol) {
    return (are_close_enough(a, 0.0f, pow(10.0,-10.0)));
};

bool is_close_to_one(double a, double tol) {
    return (are_close_enough(a, 1.0f, pow(10.0,-10.0)));
};

// Класс "Точка"
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

bool operator== (const Point& p1, const Point& p2) {
    return are_close_enough(p1.x,p2.x)&&are_close_enough(p1.y,p2.y);
};
       

