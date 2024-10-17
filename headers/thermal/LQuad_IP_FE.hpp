#pragma once
#include <cmath>
#include "../../lib/eigen-3.4.0/Eigen/Dense"
#include <iostream>


// Точки интегрирования
extern std::array<std::pair<double, double>, 2> IntP;

struct Point 
{
    double x, y;

    Point(double x, double y)
    {
        this->x = x;
        this->y = y;
    };
};

// Изопараметрический линейный четрырехугольный конечный элемент
class LQuad 
{
    private:
        std::array<std::reference_wrapper<std::pair<Point, int>>, 4> _verts; // Массив вершин с глобальной нумерацией
        Eigen::Vector<double, 8> _coords; // Вектор координат вершин элемента
        Eigen::Matrix2d _D; // Матрица коэффициентов теплопроводности
        Eigen::Matrix<double, 4, 4> _H; // Матрица теплопроводности элемента

        // Функции формы
        Eigen::RowVector<double, 4> Shape_Func(const double xi, const double eta);

        // Частные производные функций формы 
        std::pair<std::array<double, 4>, std::array<double, 4>> PD_Shape_Func(const double xi, const double eta);
        
        // Матрица градиентов
        Eigen::Matrix<double, 2, 4> Grad_Mat(const double xi, const double eta);

        // Функция отображения
        Point Mapping(const double xi, const double eta);

        // Якобиан преобразования
        Eigen::Matrix2d Jacobian(const double xi, const double eta);

    public:
        LQuad(const std::array<std::reference_wrapper<std::pair<Point, int>>, 4> Vertices, const double k1, const double k2);
        
        const std::array<std::reference_wrapper<std::pair<Point, int>>, 4> Vertices() const; 
        const Eigen::Matrix2d& D() const;
        const Eigen::Matrix<double, 4, 4>& Cond_Mat() const;    
        double Temperature(const double xi, const double eta, const double& T1, const double& T2, const double& T3, const double& T4);
};