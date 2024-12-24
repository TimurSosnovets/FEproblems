#pragma once
#include <cmath>
#include "Structs.hpp"
#include "Materials.hpp"
#include "../../lib/eigen-3.4.0/Eigen/Dense"
#include <iostream>


// Точки интегрирования
extern std::array<std::pair<double, double>, 2> IntP;



// Изопараметрический линейный четрырехугольный конечный элемент
class LQuad 
{
    private:
        Eigen::Vector<double, 8> _coords; // Вектор координат вершин элемента
        const Material _material;

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
        LQuad(const std::array<Node*, 4> v, const Material m);
        
        const double Temperature(const double xi, const double eta, Eigen::Vector<double, 4> nodal_temps) const;
        const double Element_Temp(Eigen::Vector<double, 4> nodal_temps) const;
        Eigen::Matrix<double, 4, 4> Cond_Mat(Eigen::Vector<double, 4> nodal_temps) const; // Матрица теплопроводности
        Eigen::Matrix<double, 4, 4> Damp_Mat(Eigen::Vector<double, 4> nodal_temps) const; // Матрица демфпирования 
        Eigen::Vector<double, 4> Heat_Load_Surf(const double heat_flux, const double eps, Eigen::Vector<double, 4> nodal_temps, std::array<int, 2> s) const; // Вектор узловых нагрузок
};