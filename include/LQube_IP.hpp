#pragma once
// STL
#include <cmath>
#include <iostream>
#include <optional>
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
        Eigen::Vector<double, 24> _coords; // Координаты вершин
        const Material* _material; // Материал элемента

        /*Предрасчитанные значения*/
        std::optional<std::array<Eigen::Matrix<double, 3, 8>, 8>> Grad;// Матрица градиентов
        std::optional<std::array<Eigen::Matrix<double, 8, 3>, 8>> Grad_T;// Матрица градиентов (транспонированная)
        std::optional<std::array<Eigen::RowVector<double, 8>, 8>> Shape; // Функции формы
        std::optional<std::array<Eigen::Vector<double, 8>, 8>> Shape_T; // Функции формы (транспонированные)
        std::optional<std::array<Eigen::Vector<double, 8>, 4>> Shape_surf; // Функции формы по поверхности
        std::optional<std::array<double, 8>> dJac; // Определитель якобиана преобразования
 

        /*Внутренние функции*/
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
        /*Конструктор*/
        LQube(std::vector<Node*> v, const Material* m);

        /*Предрасчёт характеристик*/
        void calculate_element();

        /*Матрицы элемента*/
        Eigen::Matrix<double, 8, 8> Cond_Mat(Eigen::Vector<double, 8>& nodal_temps) const; // Матрица теплопроводности
        Eigen::Matrix<double, 8, 8> Damp_Mat(Eigen::Vector<double, 8>& nodal_temps) const; // Матрица демфпирования (теплоёмкости)
        Eigen::Vector<double, 8> Heat_Load_Surf(const double heat_flux, const double& eps, Eigen::Vector<double, 8>& nodal_temps, double surf_area) const; // Вектор узловых нагрузок (с учётом излучения и кривизны поверхности)

        /*Числовые значения элемента*/
        const double Point_Temp(const double xi, const double eta, const double zeta, const Eigen::Vector<double, 8>& nodal_temps) const; // Температура заданной точке элемента
        const double Element_Temp(const Eigen::Vector<double, 8>& nodal_temps) const; // Репрезентативная температура элемента
};