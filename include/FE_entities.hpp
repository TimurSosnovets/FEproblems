#pragma once
// STL
#include <cmath>
#include <iostream>
#include <string>
// Current project
#include "Materials.hpp"
#include "Output.hpp"

class Isoparametric_3D;
class LQube;

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

struct Cache
{
    // Матрица градиентов
    std::vector<Eigen::MatrixXd> GM; 
    std::vector<Eigen::MatrixXd> GM_T;
    // Матрица функций форм
    std::vector<Eigen::RowVectorXd> SF;
    std::vector<Eigen::VectorXd> SF_T;
    // Матрица функций форм поверхности
    std::vector<Eigen::VectorXd> SF_s;
    // Определитель якобиана преобразования
    std::vector<double> J_det;
};

// Узел
class Node
{
    private:
        const Point point; // Непосредственно точка
        const int gn; // Глобальный номер

    public:
        // Дефолтный конструктор
        Node();

        // 2d конструктор
        Node(double x, double y, int n);

        // 3d конструктор
        Node(double x, double y, double z, int n);

        // точка-конструктор
        Node(Point p, int n);

        // Координаты
        const Point& coords() const;
        // Номер элемента
        int global_number() const;
        friend class LQube;
        friend class TFE_model;
};


// Конечный элемент
class Element
{
    private:
        std::unique_ptr<Isoparametric_3D> type; // Тип элемента
        const std::vector<Node*> vertices; // Массив ссылок на узлы - вершины
        const int gn; // Номер элемента
        const Material* material; // Материал элемента
        std::string* layer = nullptr; // Положение по слою
        std::string* primitive = nullptr; // Положение по части аппарата
        const bool is_surface; // Флаг элемента на поверхности
        const float surface_area; // Площадь повехности
        Cache cache; // Предрасчитанные значения

        bool has_cache() const;

    public:
        // Конструктор
        Element(std::unique_ptr<Isoparametric_3D> fe_type, const std::vector<Node*>& v, const int n, const Material* const m, const bool s = false, const float area = 0);

        // Указание слоя
        void set_layer(std::string* lr);

        // Указание слоя
        void set_primitive(std::string* pr);

        // Данные об элементе
        void get_info(const bool to_console, const std::string& filename) const;
        
        // Друзья
        friend class LQube;
        friend class TFE_model;
};



