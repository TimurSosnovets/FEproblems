#pragma once
// STL
#include <cmath>
#include <iostream>
#include <optional>
// Eigen
#include <Dense>
// Current project
#include "FE_entities.hpp"
#include "Materials.hpp"

// Точки интегрирования
extern std::array<std::pair<double, double>, 2> int_pnts;

// Изопараметрический линейный кубический элемент
class LQube
{
    private:
        /*Внутренние функции*/
        // Функции формы
        static Eigen::RowVector<double, 8> Shape_Func(const double xi, const double eta, const double zeta);

        // Частные производные функций формы 
        static std::array<Eigen::RowVector<double, 8>, 3> Shape_Func_PD(const double xi, const double eta, const double zeta);
        
        // Матрица градиентов
        static Eigen::Matrix<double, 3, 8> Grad_Mat(const double xi, const double eta, const double zeta);

        // Функция отображения
        static Point Mapping(const double xi, const double eta, const double zeta, const Element& FE);

        // Якобиан преобразования
        static Eigen::Matrix3d Jacobian(const double xi, const double eta, const double zeta, const Element& FE);

    public:
        /*Предрасчёт характеристик*/
        static void calculate_element(Element& FE);

        /*Матрицы элемента*/
        static Eigen::Matrix<double, 8, 8> Cond_Mat(const Element& FE, const Eigen::Vector<double, 8>& nodal_temps); // Матрица теплопроводности
        static Eigen::Matrix<double, 8, 8> Damp_Mat(const Element& FE, const Eigen::Vector<double, 8>& nodal_temps); // Матрица демфпирования (теплоёмкости)
        static Eigen::Vector<double, 8> Heat_Load_Surf(const Element& FE, const double heat_flux, const float eps, const Eigen::Vector<double, 8>& nodal_temps); // Вектор узловых нагрузок (с учётом излучения и кривизны поверхности)

        /*Числовые значения элемента*/
        static double Point_Temp(const double xi, const double eta, const double zeta, const Eigen::Vector<double, 8>& nodal_temps); // Температура заданной точке элемента
        static double Element_Temp(const Eigen::Vector<double, 8>& nodal_temps); // Репрезентативная температура элемента
};