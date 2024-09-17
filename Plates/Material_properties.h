#pragma once
#include "../toolbox/eigen-3.4.0/Eigen/Dense"

class Material {
    private:
        double _E; // Модуль упругости
        double _nu; // Коэффициент Пуассона
        Eigen::Matrix3d _D; // Матрица жесткости изотропного материала
    
    public:
        Material(const double E, const double nu): _E(E), _nu(nu) {
            _D << 1, nu, 0,
                  nu, 1, 0,
                  0, 0, (1-nu)/2;
            _D = (_E/(1 - pow(_nu, 2))) * _D;
        }

        double E() const {
            return _E;
        }

        double nu() const {
            return _nu;
        }

        Eigen::Matrix3d D() const {
            return _D;
        }

};