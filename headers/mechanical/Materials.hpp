#pragma once
#include "../../lib/eigen-3.4.0/Eigen/Dense"

class Isotropic {
    private:
        double _E; // Модуль упругости
        double _nu; // Коэффициент Пуассона
        Eigen::Matrix3d _D; // Матрица жесткости изотропного материала
        double _alpha; // КЛТР
    
    public:
        Isotropic(const double E, const double nu, const double a);
        double E() const;
        double nu() const;
        Eigen::Matrix3d D() const;
        double CLTE() const;
};