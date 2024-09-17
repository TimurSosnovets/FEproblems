#pragma once
#include "../toolbox/eigen-3.4.0/Eigen/Dense"
#include <vector>

// Вектор перемещений
class Displacement2d {
    private:
        Eigen::Vector2d _coord;

    public: 
        Displacement2d(const double u, const double v) {
            _coord << u, v;
        }

        double u() const {
            return _coord[0];
        }

        double v() const {
            return _coord[1];
        }

        Eigen::Vector2d coord() const {
            return _coord;
        }
};


// Вектор напряжений
class Stress2d {
    private:
        Eigen::Vector3d _coord;
        std::pair<double, double> _sigma_principal;

  
    public:
        Stress2d(const std::vector<double> stresses) {
            if (stresses.size() != 3) throw std::invalid_argument("Stresses must have exactly 3 values.");
            _coord << stresses[0], stresses[1], stresses[2];
            _sigma_principal.first = 0.5 * (_coord[0] + _coord[1] + sqrt(pow(_coord[0] - _coord[1], 2.0) + 4 * pow(_coord[2], 2.0)));
            _sigma_principal.second = 0.5 * (_coord[0] + _coord[1] - sqrt(pow(_coord[0] - _coord[1], 2.0) + 4 * pow(_coord[2], 2.0)));
        }

        double sigma_x() const {
            return _coord[0];
        }

        double sigma_y() const {
            return _coord[1];
        }

        double tau_xy() const {
            return _coord[3];
        }

        Eigen::Vector3d coord() const {
            return _coord;
        }

        std::pair<double, double> sigma_principal() const {
            return _sigma_principal;
        }
        
        double equivalent_stress() const{
            return ((1 / sqrt(2)) * sqrt(pow(_sigma_principal.first, 2.0) + pow(_sigma_principal.second, 2.0) + pow(_sigma_principal.first - _sigma_principal.second, 2.0)));
        }
        
};