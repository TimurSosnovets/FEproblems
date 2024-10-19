#pragma once
#include "TFEA_methods.hpp"
#include <cmath>



// Геометрия спускаемого аппарата
class DM_Geom2d
{
    private:
        const Point ORIGIN = Point(0, 0); // Начало координат
        const double X_circle, X_cone1, X_cone2, X_cyl; // Х-координаты точек окончания соответствующих поверхностей

    public:
        DM_Geom2d(const double Circle, const double Cone1, const double Cone2, const double Cylinder);
        // Окружность
        double circle(const double x, const double r, const double center = sqrt(336));
        // Прямая через две точки
        double line(const double x, const Point P1, const Point P2);

        // Возвращаемые значения
        const double x_cr() const;
        const double x_cn1() const;
        const double x_cn2() const;
        const double x_cl() const;   
};

// Конечно-элментная модель спускаемого аппарата
class DM_FEmodel
{
    private:
        const DM_Geom2d geometry; // Геометрия аппарата
        const double h_GC, h_TZMK, h_AMg; // Толщины слоёв СУ-2500, ТЗМК, АМг-6 соответственно
       
        std::vector<std::pair<Point, int>> _Nodes; // Массив узлов
        const double _DOF; // Степень свободы
        std::array<std::pair<LQuad, int>, _DOF> _FEs; // Массив конечных элементов (Ссылок на них)
        Eigen::MatrixXd GCM; // Глобальная матрица теплопроводности

        // Создание узлов

    public:
};