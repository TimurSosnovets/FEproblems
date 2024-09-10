#include "../2dGeometry/2dCircles.h"

// Точки
Point center = Point(0,0);
Point a1 = Point(20,0);
Point a2 = Point(0,20);
Point a3 = Point(-20,0);

// Значения
double radius = 20;
int n = 4;

// Окружности
Circle circ1 = Circle(center, radius);

// Полигоны
Polygon pol1 = Polygon({Point(20,0), Point(0,20), Point(-20,0), Point(0,-20)});

// Тест построения из 3х точек
bool test_factory() {
    return (circ1 == make_circle_from_points(a1, a2, a3));
};

// Тест создания полигона
bool test_to_polygon() {
    return (circ1.to_polygon(n) == pol1);
};

int main() {
std::cout << std::endl << std::endl << std::endl;

if (test_factory()) std::cout << "Success! The circle factory works correctly!" << std::endl;
else {
    std::cout << "Error detected! The circle factory works incorrectly. Here are details: \n"; 
    
    std::cout << "Circle 1: ";
    circ1.get_info();

    std::cout << "Point 1: ";
    a1.get_info();
    std::cout << std::endl;

    std::cout << "Point 2: ";
    a2.get_info();
    std::cout << std::endl;

    std::cout << "Point 3: ";
    a3.get_info();
    std::cout << std::endl;

    std::cout << "Circle from points 1-3: ";
    make_circle_from_points(a1, a2, a3).get_info();
}
std::cout << std::endl;

if (test_to_polygon()) std::cout << "Success! The to_polygon method works correctly!" << std::endl;
else {
    std::cout << "Error detected! The to_polygon method works incorrectly. Here are details: \n"; 
    
    std::cout << "Polygon from circle 1: ";
    circ1.to_polygon(n).get_info();
    
    std::cout << "Expected polygon: ";
    pol1.get_info();
}
std::cout << std::endl;

return 0;
};