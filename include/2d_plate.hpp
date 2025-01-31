#pragma once
#include <numeric>
#include <cmath>
#include "Structs.hpp"
#include "Materials.hpp"
#include "LQuad_IP.hpp"
#include "../lib/Eigen/Dense"
#include <iostream>

// Точки интегрирования
extern std::array<std::pair<double, double>, 2> IntP;

// Класс плоская слоистая пластина
class LPlate
{
    private:
        const double width; // Ширина пластины
        const std::array<double, 3> hight; // Толщины слоёв
        const std::array<Material, 3> materials; // Материал слоёв
        const double thickness = 1; // Толщина пластины

        std::vector<Node> _nodes; // Массив узлов
        std::vector<Element>  _elements; // Массив элементов
        const double _DOF; // Степень свободы


        void nodes_creation(const int W, const std::array<int, 3> L_h); // Создание узлов
        void elements_creation(const int W, const std::array<int, 3> L_h); // Создание элементов

    public:
        // Конструктор
        LPlate(const std::array<Material, 3> Layer_materials, const double Width, const std::array<double, 3> Layer_hights, const int FEC_width, const std::array<int, 3> FEC_layer);
        
        // Возвращаемые значения
        const std::vector<Node>& Nodes() const;
        const std::vector<Element>& Elements() const;
        Eigen::MatrixXd GCM(Eigen::VectorXd nodal_temps) const; // Глобальная матрица теплопроводности
        Eigen::MatrixXd GDM(Eigen::VectorXd nodal_temps) const; // Глобальная матрица демпфирования
        Eigen::VectorXd F(const double q, const double eps, Eigen::VectorXd nodal_temps) const; // Вектор узловых нагрузок

    
};