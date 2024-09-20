#include "../../2dGeometry/2dPolygon.h"

using namespace std;

// Массив вершин
std::vector<Point> points_set = {Point(0,0), Point(30,0), Point(0,30)};
// Полигоны
Polygon pol1 = Polygon(points_set);
// Точки
Point outs = Point(15,20);
Point ins = Point(15,10);
Point vert3 = Point(0,30);

// Тест сторон
bool test_sides() {
    std::vector<Segment> expected = {Segment(points_set[0], points_set[1]), Segment(points_set[1], points_set[2]), Segment(points_set[2], points_set[0])};
    if (pol1.get_sides() == expected) return true;
    else return false;
};

// Тест центральной точки
bool test_centroid() {
    Point expected = Point(10,10);
    if (pol1.get_centroid() == expected) return true;
    else return false;
}

// Тест содержания точки
bool test_doesnt_contain_point() {
    return !(pol1.contains_point(outs));
};

bool test_contains_point() {
    return pol1.contains_point(ins);
};

bool test_contains_vertex() {
    return pol1.contains_point(vert3);
};

 int main() {
cout << endl << endl << endl;

if (test_sides()) cout <<  "Success! The sides property works correctly!" << endl;
    else {
        cout << "Error detected! The sides property works incorrectly. Here are details: \n";
        pol1.get_info();
    };
cout << endl;

if (test_centroid()) cout <<  "Success! The centroid property works correctly!" << endl;
    else {
        cout << "Error detected! The centroid property works incorrectly. Here are details: \n";
        pol1.get_info();
        cout << "Centroid is ";
        if (pol1.get_centroid().has_value()) pol1.get_centroid() -> get_info();
        else cout << "Centroid is not set";
        cout << endl;
    };
cout << endl;

if ((test_contains_point()) && (test_doesnt_contain_point())) cout <<  "Success! The contains_point method works correctly!" << endl;
    else {
        cout << "Error detected! The contains_point method works incorrectly. Here are details: \n";
        pol1.get_info();

        cout << "Inside point: ";
        ins.get_info();
        cout << " Does contain?: ";
        if (pol1.contains_point(ins)) cout << "Yes." << endl;
        else cout << "No." << endl;

        cout << "Outside point: ";
        outs.get_info();
        cout << " Does contain?: ";
        if (pol1.contains_point(outs)) cout << "Yes." << endl;
        else cout << "No." << endl;

        cout << "Vertex point: ";
        vert3.get_info();
        cout << " Does contain?: ";
        if (pol1.contains_point(vert3)) cout << "Yes." << endl;
        else cout << "No." << endl;
    };
cout << endl;
return 0;
};