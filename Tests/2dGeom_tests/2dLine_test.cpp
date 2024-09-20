#include "../../2dGeometry/2dLine.h"

// Линии
Line l1 = Line(Point(0,0), Vector(1,1));
Line l2 = Line(Point(10,10), Vector(1,1));
Line l3 = Line(Point(50,0), Vector(0,1));
Line l4 = Line(Point(0,30), Vector(1,0));
// Точки

// Тест пересечения
bool test_parallel_lines_no_intersection() {
    return !(l1.is_intersection_with(l2));
};

bool test_lines_intersection() {
    Point expected = Point(50,30);
    if ((l3.is_intersection_with(l4)) && (l3.intersection_with(l4) == expected)) return true;
    else return false; 
};

int main() {
if ((test_parallel_lines_no_intersection()) && (test_lines_intersection())) cout << "Success! The intersection_with method works correctly!" << endl;
    else { 
        cout << "Error detected! The intersection_with method works incorrectly. Here are details: \n"; 
        cout << "Line 1: ";
        l1.get_info(); 

        cout << "Line 2: ";
        l2.get_info();

        cout << "Is intersection between lines 1 and 2?: ";
        if (l1.is_intersection_with(l2)) {
            std::cout << "Yes: ";
            l1.intersection_with(l2).get_info(); cout << endl;
        }
        else {
            std::cout << "No." << endl;
            cout << "t = " << l1.p_intersection_coord(l2) << ";  t2 = " << l2.p_intersection_coord(l1) << ";\n";
        };

        cout << "Line 3: ";
        l3.get_info();

        cout << "Line 4: ";
        l4.get_info();

        cout << "Is intersection between lines 3 and 4?: ";
        if (l3.is_intersection_with(l4)) {
            std::cout << "Yes: ";
            l3.intersection_with(l4).get_info(); cout << endl;
        }
        else {
            std::cout << "No." << endl;
            cout << "t = " << l3.p_intersection_coord(l4) << ";  t2 = " << l3.p_intersection_coord(l4) << ";\n";
        };
    }; 
   cout << endl;
return 0;
};