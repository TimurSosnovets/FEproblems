#pragma once
#include "Materials.hpp"
#include "2dPoint.hpp"

// Треугольный конечный элемент
class TriangleFE {
    private:
        std::vector<std::pair<Point, int>> _vertices; // Вершины и глобальная нумерация
        Eigen::Matrix<double, 6, 6> _K; // Матрица жесткости элемента
        Eigen::Matrix<double, 3, 6> _B; // Матрица функций деформаций 
        Isotropic _material; // Материал элемента
        double _h; // Толщина
        double _square; // Площадь 

    public:
        std::vector<std::pair<Point, int>> verts() const;
        Eigen::Matrix<double, 6, 6> K() const;
        Eigen::Matrix<double, 3, 6> B() const;
        Isotropic Mat() const;
        double Square() const;        
        TriangleFE(const std::vector<std::pair<Point, int>> Vertices, const Isotropic material, const double h);
};