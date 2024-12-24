#pragma once
#include <cmath>
#include "Structs.hpp"
#include "Materials.hpp"
#include "../../lib/eigen-3.4.0/Eigen/Dense"
#include <iostream>


// Точки интегрирования
extern std::array<std::pair<double, double>, 2> IntP;



// Изопараметрический линейный четрырехугольный конечный элемент
class NEW_LQuad 
{
    private:
        const std::array<Node&, 4> _Vertices; // Вершины
        const int _Number; // Номер элемента
        Eigen::Vector<double, 8> _coords; // Вектор координат вершин элемента
        const Material mat;
        Eigen::Matrix<double, 4, 4> _H; // Матрица теплопроводности элемента
        double T_rep; // Репрезентативная температура

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
        NEW_LQuad(const int N, const std::array<Node&, 4> Vertices, const Material M, std::array<double, 4> ND_T);
        
        const const std::array<Node&, 4> Vertices() const; 
        const Eigen::Matrix2d& D() const;
        const Eigen::Matrix<double, 4, 4>& Cond_Mat() const;   
        double Temperature(const double xi, const double eta, const double& T1, const double& T2, const double& T3, const double& T4) const;
        Eigen::Vector<double, 4> Heat_Load(const double heat_flux) const;
};