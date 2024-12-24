#pragma once
#include "Materials.hpp"
#include <cmath>

struct Point 
{
    const double x, y;

    Point() : x(0.0), y(0.0) {} // Initializes x and y to 0.0
    Point(double X, double Y) : x(X), y(Y) {} 

    static double distance_to(const Point& p1, const Point& p2) 
    {
        return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
    }
};

// Узел
struct Node
{
    const Point point; // Непосредственно точка
    const int gn; // Глобальный номер

    Node() : point(Point(0,0)), gn(0) {}
    Node(Point p, int n) : point(p), gn(n) {}
    Node(double x, double y, int n) : point(Point(x,y)), gn(n) {}
};

// Элемент
struct Element
{
    const std::vector<Node*> vertices; // Массив ссылок на узлы - вершины
    const int gn; // Номер элемента
    const Material material; // Материал элемента
    const bool surface; // Флаг элемента на поверхности

    Element(std::vector<Node*> v, int n, Material m, bool s = false) : vertices(v), gn(n), material(m), surface(s) {}
};

