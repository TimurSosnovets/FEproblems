#pragma once
#include <cmath>
#include <iostream>
#include "2dVector.h"
using namespace std;

//Класс "Точка"
class Point {
    private:
        double x;
        double y;
    public:
        friend class Circle;
        friend class Polygon;
        friend class Vector;
        friend class Rect;
        friend bool are_close_enough (double a, double b, double tol);
        friend Point operator+ (const Point& p1, const Point& p2);
        friend Vector operator- (const Point& p1, const Point& p2);
        friend bool operator== (const Point& p1, const Point& p2);
        
        void get_info () {
            cout << "Point: " << "(" << x << ", " << y << ").";
        }

        explicit Point(double x, double y) : x(x), y(y) { }

        Point displaced (const Vector& V, double times=1.0f) const {
            return Point(x+V.u*times, y+V.v*times);
        }
        double distance_to (const Point& p) const {
            return sqrt(pow(x-p.x,2.0f)+pow(y-p.y,2.0f));
        };
};

Point operator+ (const Point& p1, const Point& p2) {
    return Point (p1.x+p2.x, p1.y+p2.y);
};
bool operator== (const Point& p1, const Point& p2) {
    return are_close_enough(p1.x,p2.x)&&are_close_enough(p1.y,p2.y);
};
Vector operator- (const Point& p1, const Point& p2) {
      return Vector (p1.x-p2.x, p1.y-p2.y);
};        

