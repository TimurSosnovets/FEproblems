#pragma once
#include <cmath>

struct Point 
{
    double x, y;

    Point() : x(0.0), y(0.0) {} // Initializes x and y to 0.0
    Point(double x, double y)
    {
        this->x = x;
        this->y = y;
    };
    static double distance_to(const Point& p1, const Point& p2) 
    {
        return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
    }
};

// Узел
struct Node
{
    Point pnt; // Непосредственно точка
    int GN; // Глобальный номер

    Node() : pnt(Point(0,0)), GN(0) {}
    Node(Point p, int n) : pnt(p), GN(n) {}
};