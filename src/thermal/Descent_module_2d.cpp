#include "../../headers/thermal/Descent_module_2d.hpp"


// ГЕОМЕТРИЯ СПУСКАЕМОГО АППАРАТА
// Конструктор класса
DM_Geom2d::DM_Geom2d(const double Circle, const double Cone1, const double Cone2, const double Cylinder) : X_circle(Circle), X_cone1(Cone1), X_cone2(Cone2), X_cyl(Cylinder) {};

// Х-координата окружности
double DM_Geom2d::circle(const double x, const double r, const double center = sqrt(336)) 
{
    return sqrt(r - pow((x - center), 2));
}

// X-координата прямой, проходящей через две точки
double DM_Geom2d::line(const double x, const Point P1, const Point P2)
{
    return (((P2.y - P1.y) / (P2.x - P1.x)) * (x - P1.x) + P1.y);
}

// Возвращаемые значения
const double DM_Geom2d::x_cr() const
{
    return X_circle;
}

const double DM_Geom2d::x_cn1() const
{
    return X_cone1;
}

const double DM_Geom2d::x_cn2() const
{
    return X_cone2;
}

const double DM_Geom2d::x_cl() const
{
    return X_cyl;
}
