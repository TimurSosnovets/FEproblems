#pragma once
#include "../../headers/mechanical/Plate_methods.hpp"

//Пластина, разбитая на треугольные КЭ
class Plate_triangulated {
    private:
        double _sideX; // Длина стороны, параллельной OX
        double _sideY; // Длина стороны, параллельной OY
        Isotropic _material; // Материал пластины
        double _h; // Толщина пластины
        std::vector<std::pair<Point, int>> _nodes; // Узлы и их глобальная нумерация        
        std::vector<std::pair<TriangleFE, int>> _finite_els; // Конечные элементы и их номера
        int _DoF; // Степени свободы (размер глобальной матрицы жесткости)
        Eigen::MatrixXd _global_SM; // Глобальная матрица жесткости

    public:
        Plate_triangulated(const double lenX, const double lenY, const int m, const int n, const double thick, const Isotropic mater);        
        std::vector<std::pair<TriangleFE, int>> FEs() const;
        std::vector<std::pair<Point, int>> NDs() const;
        Eigen::MatrixXd GSM() const;
        double thickness() const;
};