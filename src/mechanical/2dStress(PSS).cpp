#include "../../headers/mechanical/2dStress(PSS).hpp"

Stress2d::Stress2d(const Eigen::Vector3d& stresses) {
    if (stresses.size() != 3) throw std::invalid_argument("Stresses must have exactly 3 values.");
    _coord = stresses;
    _sigma_principal.first = 0.5 * (_coord[0] + _coord[1] + sqrt(pow(_coord[0] - _coord[1], 2.0) + 4 * pow(_coord[2], 2.0)));
    _sigma_principal.second = 0.5 * (_coord[0] + _coord[1] - sqrt(pow(_coord[0] - _coord[1], 2.0) + 4 * pow(_coord[2], 2.0)));
}

double Stress2d::sigma_x() const {
    return _coord[0];
}

double Stress2d::sigma_y() const {
    return _coord[1];
}

double Stress2d::tau_xy() const {
    return _coord[3];
}

Eigen::Vector3d Stress2d::coord() const {
    return _coord;
}

std::pair<double, double> Stress2d::sigma_principal() const {
    return _sigma_principal;
}

double Stress2d::equivalent_stress() const{
    return ((1 / sqrt(2)) * sqrt(pow(_sigma_principal.first, 2.0) + pow(_sigma_principal.second, 2.0) + pow(_sigma_principal.first - _sigma_principal.second, 2.0)));
}

