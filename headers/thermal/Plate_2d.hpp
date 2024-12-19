#pragma once
#include "TFEA_methods.hpp"
#include "Materials.hpp"
#include <cmath>
#include <optional>

// Точки интегрирования
extern std::array<std::pair<double, double>, 2> IntP;
const std::size_t MAX_DOF = 5000;

// Класс плоская пластина
class Plate_2d
{
    private:
        const double L_x; // Длина стороны по X
        const double L_y; // Длина стороны по Y
        const Material mat; // Материал

        std::vector<std::pair<Point, int>> _Nodes; // Массив узлов
        const double _DOF; // Степень свободы
        std::array<std::optional<std::pair<LQuad, int>>, MAX_DOF> _FEs; // Массив конечных элементов (Ссылок на них)
        Eigen::MatrixXd _GCM; // Глобальная матрица теплопроводности

        void nodes_creation(const int m, const int n); // Создание узлов
        void elements_creation(const int m, const int n); // Создание ээлементов

    public:
        // Конструктор
        Plate_2d(const Material material, const double length_x, const double length_y, const int quant_x, const int quant_y);
        
        // Возвращаемые значения
        const std::vector<std::pair<Point, int>>& Nodes() const;
        const std::array<std::optional<std::pair<LQuad, int>>, MAX_DOF>& FEs() const;
        const Eigen::MatrixXd& GCM() const;

    
};