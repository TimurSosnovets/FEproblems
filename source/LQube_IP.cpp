#include "LQube_IP.hpp"

// Точки интегрирования и весовые коэффициенты
std::array<std::pair<double, double>, 2> IntP = {{ {1/sqrt(3), 1.0}, {-1/sqrt(3), 1.0} }};

// Функции формы
Eigen::RowVector<double, 8> LQube::Shape_Func(const double xi, const double eta, const double zeta) const
{
    Eigen::RowVector<double, 8> N;

    N[0] = (1/8.0) * (1 - eta) * (1 - xi)*(1-zeta);
    N[1] = (1/8.0) * (1 - eta) * (1 + xi)*(1-zeta);
    N[2] = (1/8.0) * (1 + eta) * (1 + xi)*(1-zeta);
    N[3] = (1/8.0) * (1 + eta) * (1 - xi)*(1-zeta);
    N[4] = (1/8.0) * (1 - eta) * (1 - xi)*(1+zeta);
    N[5] = (1/8.0) * (1 - eta) * (1 + xi)*(1+zeta);
    N[6] = (1/8.0) * (1 + eta) * (1 + xi)*(1+zeta);
    N[7] = (1/8.0) * (1 + eta) * (1 - xi)*(1+zeta);
    
    return N;
}