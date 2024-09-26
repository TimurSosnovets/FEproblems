#include "../../headers/mechanical/Materials.hpp"

Isotropic::Isotropic(const double E, const double nu, const double a): _E(E), _nu(nu), _alpha(a) {
    _D << 1, nu, 0,
    nu, 1, 0,
    0, 0, (1-nu)/2;
    _D = (_E/(1 - pow(_nu, 2))) * _D;
}

double Isotropic::E() const {
    return _E;
}

double Isotropic::nu() const {
    return _nu;
}

Eigen::Matrix3d Isotropic::D() const {
    return _D;
}

double Isotropic::CLTE() const {
    return _alpha;
}
