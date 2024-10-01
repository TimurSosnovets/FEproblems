#pragma once
#include "../../lib/eigen-3.4.0/Eigen/Dense"
#include <vector>

// Вектор напряжений
class Stress2d {
    private:
        Eigen::Vector3d _coord;
        std::pair<double, double> _sigma_principal;  
    public:
        Stress2d(const Eigen::Vector3d& stresses);
        double sigma_x() const;
        double sigma_y() const;
        double tau_xy() const;
        Eigen::Vector3d coord() const;
        std::pair<double, double> sigma_principal() const;        
        double equivalent_stress() const;        
};