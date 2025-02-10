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
extern std::array<std::pair<double, double>, 2> int_pnts;

// Изопараметрический линейный кубический элемент
class LQube
{
    private:
        const Eigen::Vector<double, 24> _coords; // Координаты вершин
        const Material _material; // Материал элемента
        const bool is_surface; // Маркер поверхностного элемента (поверность всегда - узлы 1-4)

        // Функции формы
        Eigen::RowVector<double, 8> Shape_Func(const double xi, const double eta, const double zeta) const;

        // Частные производные функций формы 
        std::array<Eigen::RowVector<double, 8>, 3> Shape_Func_PD(const double xi, const double eta, const double zeta) const;
        
        // Матрица градиентов
        Eigen::Matrix<double, 3, 8> Grad_Mat(const double xi, const double eta, const double zeta) const;

        // Функция отображения
        Point Mapping(const double xi, const double eta, const double zeta) const;

        // Якобиан преобразования
        Eigen::Matrix3d Jacobian(const double xi, const double eta, const double zeta) const;

    public:
        // Конструктор
        LQube(const std::vector<Node*> v, const Material& m, const bool surf);

        Eigen::Matrix<double, 8, 8> Cond_Mat(const Eigen::Vector<double, 24>& nodal_temps) const; // Матрица теплопроводности
        Eigen::Matrix<double, 8, 8> Damp_Mat(const Eigen::Vector<double, 24>& nodal_temps) const; // Матрица демфпирования (теплоёмкости)
        Eigen::Vector<double, 24> Heat_Load_Surf(const double heat_flux, const double& eps, Eigen::Vector<double, 24>& nodal_temps, const double Jacobian) const; // Вектор узловых нагрузок (с учётом излучения и кривизны поверхности)

        // Возвращаемые значения
        const double Point_Temp(const double xi, const double eta, const double zeta, const Eigen::Vector<double, 24>& nodal_temps) const; // Температура заданной точке элемента
        const double Element_Temp(const Eigen::Vector<double, 24>& nodal_temps) const; // Репрезентативная температура элемента
};