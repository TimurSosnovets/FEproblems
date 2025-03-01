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

// Базовый класс конечного элемента
class Isoparametric_3D
{
    protected:
        /*Внутренние функции*/
        // Функции формы
        virtual Eigen::RowVectorXd Shape_Func(const double xi, const double eta, const double zeta) const = 0;

        // Частные производные функций формы 
        virtual std::array<Eigen::RowVectorXd, 3> Shape_Func_PD(const double xi, const double eta, const double zeta) const = 0;
        
        // Матрица градиентов
        virtual Eigen::MatrixXd Grad_Mat(const double xi, const double eta, const double zeta) const = 0;

        // Функция отображения
        virtual Point Mapping(const double xi, const double eta, const double zeta, const Element& FE) const = 0;

        // Якобиан преобразования
        virtual Eigen::Matrix3d Jacobian(const double xi, const double eta, const double zeta, const Element& FE) const = 0;

    public:
        virtual ~Isoparametric_3D() = default;

        /*Предрасчёт характеристик*/
        virtual void calculate_element(Element& FE) const = 0;

        /*Матрицы элемента*/
        // Матрица теплопроводности
        virtual Eigen::MatrixXd Cond_Mat(const Element& FE, const Eigen::VectorXd& nodal_temps) const = 0; 
        // Матрица демфпирования (теплоёмкости)
        virtual Eigen::MatrixXd Damp_Mat(const Element& FE, const Eigen::VectorXd& nodal_temps) const = 0; 
        // Вектор узловых нагрузок (с учётом излучения и кривизны поверхности)
        virtual Eigen::VectorXd Heat_Load_Surf(const Element& FE, const double heat_flux, const float eps, const Eigen::VectorXd& nodal_temps) const = 0; 

        /*Числовые значения элемента*/
        // Температура заданной точке элемента
        virtual double Point_Temp(const double xi, const double eta, const double zeta, const Eigen::VectorXd& nodal_temps) const = 0; 
        // Репрезентативная температура элемента
        virtual double Element_Temp(const Eigen::VectorXd& nodal_temps) const = 0; 
};

enum class ElementType 
{
    LQube,
    LQuad
};