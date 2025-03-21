#pragma once
// STL
#include <cmath>
#include <iostream>
#include <optional>
// Eigen
#include <Dense>
// Current project
#include "FE_entities.hpp"
#include "FE_instances.hpp"
#include "Materials.hpp"

// Точки интегрирования
extern std::array<std::pair<double, double>, 2> linear_int;
extern std::array<std::pair<double, double>, 3> triang_int;

// Изопараметрический линейный кубический элемент (xi - L1; eta - L2;)
class LWedge : public Isoparametric_3D
{
    private:
        /*Внутренние функции*/
        // Функции формы
        Eigen::RowVectorXd Shape_Func(const double xi, const double eta, const double zeta) const override;

        // Частные производные функций формы 
        std::array<Eigen::RowVectorXd, 3> Shape_Func_PD(const double xi, const double eta, const double zeta) const override;
        
        // Матрица градиентов
        Eigen::MatrixXd Grad_Mat(const double xi, const double eta, const double zeta) const override;

        // Функция отображения
        Point Mapping(const double xi, const double eta, const double zeta, const Element& FE) const override;

        // Якобиан преобразования
        Eigen::Matrix3d Jacobian(const double xi, const double eta, const double zeta, const Element& FE) const override;

    public:
        /*Предрасчёт характеристик*/
        void calculate_element(Element& FE) const override;

        /*Матрицы элемента*/
        // Матрица теплопроводности
        Eigen::MatrixXd Cond_Mat(const Element& FE, const Eigen::VectorXd& nodal_temps) const override;
        // Матрица демфпирования (теплоёмкости) 
        Eigen::MatrixXd Damp_Mat(const Element& FE, const Eigen::VectorXd& nodal_temps) const override;
        // Вектор узловых нагрузок (с учётом излучения и кривизны поверхности) 
        Eigen::VectorXd Heat_Load_Surf(const Element& FE, const double heat_flux, const float eps, const Eigen::VectorXd& nodal_temps) const override; 

        /*Числовые значения элемента*/
        // Температура заданной точке элемента
        double Point_Temp(const double xi, const double eta, const double zeta, const Eigen::VectorXd& nodal_temps) const override;
        // Репрезентативная температура элемента 
        double Element_Temp(const Eigen::VectorXd& nodal_temps) const override; 
};