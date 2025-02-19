#pragma once
// STL
#include <cmath>
#include <iostream>
#include <string>
// Current project
#include "Materials.hpp"
#include "Output.hpp"


struct Point 
{
    const double x, y, z;

    // Дефолтный конструктор
    Point() : x(0.0), y(0.0), z(0.0) {}

    // 2d конструктор
    Point(double X, double Y) : x(X), y(Y), z(0.0) {} 

    //3d конструктор
    Point(double X, double Y, double Z) : x(X), y(Y), z(Z) {}

    static double distance(const Point& p1, const Point& p2) 
    {
        return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2) + pow(p2.z - p1.z, 2));
    }
};

// Узел
struct Node
{
    const Point point; // Непосредственно точка
    const int gn; // Глобальный номер

    // Дефолтный конструктор
    Node() : point(), gn(0) {}

    // 2d конструктор
    Node(double x, double y, int n) : point(Point(x,y)), gn(n) {}

    // 3d конструктор
    Node(double x, double y, double z, int n) : point(Point(x,y,z)), gn(n) {}

    // точка-конструктор
    Node(Point p, int n) : point(p), gn(n) {}
};

// Элемент
struct Element
{
    const std::vector<Node*> vertices; // Массив ссылок на узлы - вершины
    const int gn; // Номер элемента
    const Material* material; // Материал элемента
    const bool is_surface; // Флаг элемента на поверхности
    const double surface_area; // Площадь повехности
    std::string* layer = nullptr; // Положение по слою
    std::string* primitive = nullptr; // Положение по части аппарата
    

    // Конструктор
    Element(std::vector<Node*> v, int n, const Material* m, bool s = false, double area = 0) : vertices(v), gn(n), material(m), is_surface(s), surface_area(area) {}

    // Указание слоя
    void set_layer(std::string* lr)
    {
        layer = lr;
    }

    // Указание слоя
    void set_primitive(std::string* pr)
    {
        primitive = pr;
    }

    void get_info(const size_t DOF, const std::string& filename) const
    {   
        /*Инициализация*/
        std::string message, m_surface, m_layer, m_primitive, nodes;
        
        /*Определение качественных характеристик*/
            // Является ли поверхностным
            m_surface = is_surface ? "surface" : "internal";
            // В каком слое находится
            m_layer = (layer == nullptr) ? "not assigned" : *layer;
            // Частью какой геометрии является
            m_primitive = (primitive == nullptr) ? "not assigned" : *primitive;      

        /*Массив номеров узлов через пробел*/
        for (size_t i = 0; i < vertices.size(); ++i)
        {
            nodes += std::to_string(vertices[i]->gn);
            if (i < vertices.size() - 1) { nodes += " "; }
        }

        /*Непосредственно сообщение*/
        message = "Element " + std::to_string(gn) + ": " + m_surface + ", layer - " + m_layer + ", primitive - " + m_primitive + ", nodes {" + nodes + "}.";
        
        /*Вывод сообщения*/
        if (DOF < 31) {logger::log(message);}
        else {logger::log(message, false, filename)};
    }
};

