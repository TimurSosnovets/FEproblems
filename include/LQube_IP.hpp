#pragma once
// STL
#include <cmath>
#include <iostream>
// Eigen
#include <Dense>
// Current project
#include "Structs.hpp"
#include "Materials.hpp"

// Точки интегрирования
extern std::array<std::pair<double, double>, 2> int_pnt_full, int_pnt_red;

// Изопараметрический линейный кубический элемент
class LQube
{
    private:
        // Функции формы
        Eigen::RowVector<double, 4> Shape_Func(const double xi, const double eta) const;

        // Частные производные функций формы 
        std::pair<std::array<double, 4>, std::array<double, 4>> PD_Shape_Func(const double xi, const double eta) const;
        
        // Матрица градиентов
        Eigen::Matrix<double, 2, 4> Grad_Mat(const double xi, const double eta) const;

        // Функция отображения
        Point Mapping(const double xi, const double eta) const;

        // Якобиан преобразования
        Eigen::Matrix2d Jacobian(const double xi, const double eta) const;
    public:
};