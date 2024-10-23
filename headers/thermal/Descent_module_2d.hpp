#pragma once
#include "TFEA_methods.hpp"
#include <cmath>
#include <optional>

const std::size_t MAX_DOF = 5000;
const double Lamda_GC = 1, Lambda_TZMK = 1, Lambda_AMg = 1;



// Геометрия спускаемого аппарата
class DM_Geom2d
{
    private:
        const Point ORIGIN = Point(0, 0); // Начало координат
        const double X_circle, X_cone1, X_cone2, X_cyl; // Х-координаты точек окончания соответствующих поверхностей
        const double R_circle, R_cone1, R_cone2; // Радиусы соовтетствующих поверхностей

    public:
        DM_Geom2d(const double R_cr, const double R_c1, const double R_c2, const double X_cr, const double X_c1, const double X_c2, const double X_cl);
        // Окружность
        static double circle(const double x, const double r, const double center = 336);
        // Прямая через две точки
        static double line(const double x, const Point P1, const Point P2);

        // Возвращаемые значения
        const double x_cr() const;
        const double x_cn1() const;
        const double x_cn2() const;
        const double x_cl() const;
        const double r_cr() const;
        const double r_cn1() const;
        const double r_cn2() const;   
};

// Конечно-элментная модель спускаемого аппарата
class DM_FEmodel
{
    private:
        const DM_Geom2d geometry; // Геометрия аппарата
        const double h_GC, h_TZMK, h_AMg; // Толщины слоёв СУ-2500, ТЗМК, АМг-6 соответственно
       
        std::vector<std::pair<Point, int>> _Nodes; // Массив узлов
        const double _DOF; // Степень свободы
        std::array<std::pair<LQuad, int>, MAX_DOF> _FEs; // Массив конечных элементов (Ссылок на них)
        Eigen::MatrixXd _GCM; // Глобальная матрица теплопроводности

        // Создание узлов
        void nodes_creation(std::vector<std::pair<Point, int>>& Nodes, const int N_cr, const int N_cn1, const int N_cn2, const int N_cl, const int N_gc, const int N_tzmk);
        void elements_creation(std::array<std::pair<LQuad, int>, MAX_DOF>& FEs, const int& FE_count);
    public:
        DM_FEmodel(DM_Geom2d& geom, const double GC, const double TZMK, const double AMg,
                   const int N_cr, const int N_cn1, const int N_cn2, const int N_cl, const int N_gc, const int N_tzmk);
        
        // Возвращаемые значения
        const std::vector<std::pair<Point, int>>& Nodes() const;
        const std::array<std::pair<LQuad, int>, MAX_DOF>& FEs() const;
        const Eigen::MatrixXd& GCM() const;
};      