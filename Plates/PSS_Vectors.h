#pragma once
#include "../2dGeometry/2dVectors.h"
#include <vector>

// Вектор перемещений
class Displacement: public Vector {
    private:
        std::vector<double> _coord;
    public:
        Displacement(const double _u, const double _v): Vector (_u, _v) {
            _coord[0] = _u;
            _coord[1] = _v;
        } 

        double U() const {
            return u;
        }

        double V() const {
            return v;
        }
};

// Вектор напряжений 
class Stress: public Vector {
    private:
        double _sigma_x;
        double _sigma_y;
        double _tau_xy;
        std::vector<double> _coord;
        std::pair<double, double> _sigma_principal;

 // Вспомогательная функция для вычисления главных напряжений
        std::pair<double, double> calculate_principal_stresses() {
            double sigma_1 = 0.5 * (_sigma_x + _sigma_y + sqrt(pow(_sigma_x - _sigma_y, 2.0) + 4 * pow(_tau_xy, 2.0)));
            double sigma_2 = 0.5 * (_sigma_x + _sigma_y - sqrt(pow(_sigma_x - _sigma_y, 2.0) + 4 * pow(_tau_xy, 2.0)));
            return std::make_pair(sigma_1, sigma_2);
        }

    public:
        Stress(const std::vector<double> stresses):
         _coord(stresses),
         _sigma_x(_coord[0]),
         _sigma_y(_coord[1]),
         _tau_xy(_coord[2]),
         _sigma_principal(calculate_principal_stresses()), 
         Vector(_sigma_principal.first, _sigma_principal.second) 
         {
            // _sigma_principal.first = (1/2) * (_sigma_x + _sigma_y + sqrt(pow(_sigma_x - _sigma_y, 2.0) + 4 * pow(_tau_xy, 2.0)));
            // _sigma_principal.second = (1/2) * (_sigma_x + _sigma_y - sqrt(pow(_sigma_x - _sigma_y, 2.0) + 4 * pow(_tau_xy, 2.0)));
            // u = _sigma_principal.first;
            // v = _sigma_principal.second;
        }

        double sigma_x() const {
            return _sigma_x;
        }

        double sigma_y() const {
            return _sigma_y;
        }

        double tau_xy() const {
            return _tau_xy;
        }
        
        std::pair<double,double> principal_stresses() const {
            return _sigma_principal;
        }

        double equivalent_stress() {
            return ((1 / sqrt(2)) * sqrt(pow(_sigma_principal.first, 2.0) + pow(_sigma_principal.second, 2.0) + pow(_sigma_principal.first - _sigma_principal.second, 2.0)));
        }
};