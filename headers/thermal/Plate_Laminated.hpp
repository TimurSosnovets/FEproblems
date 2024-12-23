#pragma once
#include "TFEA_methods.hpp"
#include "Materials.hpp"
#include <cmath>
#include <optional>

// Точки интегрирования
extern std::array<std::pair<double, double>, 2> IntP;
const std::size_t MAX_DOF = 5000;

// Класс плоская слоистая пластина
class LPlate
{
    private:
        const double Wdth; // Ширина пластины
        const std::array<double, 3> Hght; // Толщины слоёв
        const std::array<Material, 3> Mtrl; // Материал слоёв
        const double Thns = 1; // Толщина пластины

        std::vector<std::pair<Point, int>> _Nodes; // Массив узлов
        // std::vector<std::pair<int, std::array<,4>>
        const double _DOF; // Степень свободы
        std::array<std::optional<std::pair<LQuad, int>>, MAX_DOF> _FEs; // Массив конечных элементов
        Eigen::MatrixXd _GCM; // Глобальная матрица теплопроводности

        void nodes_creation(const int W, const std::array<int, 3> L_h); // Создание узлов
        void elements_creation(const int W, const std::array<int, 3> L_h); // Создание элементов

    public:
        // Конструктор
        LPlate(const std::array<Material, 3> Layer_materials, const double Width, const std::array<double, 3> Layer_hights, const int FEC_width, const std::array<int, 3> FEC_layer);
        
        // Возвращаемые значения
        const std::vector<std::pair<Point, int>>& Nodes() const;
        const std::array<std::optional<std::pair<LQuad, int>>, MAX_DOF>& FEs() const;
        const Eigen::MatrixXd& GCM() const;

    
};