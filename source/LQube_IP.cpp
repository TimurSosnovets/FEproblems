#include "LQube_IP.hpp"

// Точки интегрирования и весовые коэффициенты
std::array<std::pair<double, double>, 2> IntP = {{ {1/sqrt(3), 1.0}, {-1/sqrt(3), 1.0} }};

// Функции формы
Eigen::RowVector<double, 8> LQube::Shape_Func(const double xi, const double eta, const double zeta) const
{
    /*Инициализация*/
    Eigen::RowVector<double, 8> N;

    N[0] = (1/8.0) * (1 - eta) * (1 - xi) * (1-zeta);
    N[1] = (1/8.0) * (1 - eta) * (1 + xi) * (1-zeta);
    N[2] = (1/8.0) * (1 + eta) * (1 + xi) * (1-zeta);
    N[3] = (1/8.0) * (1 + eta) * (1 - xi) * (1-zeta);
    N[4] = (1/8.0) * (1 - eta) * (1 - xi) * (1+zeta);
    N[5] = (1/8.0) * (1 - eta) * (1 + xi) * (1+zeta);
    N[6] = (1/8.0) * (1 + eta) * (1 + xi) * (1+zeta);
    N[7] = (1/8.0) * (1 + eta) * (1 - xi) * (1+zeta);
    
    return N;
}

// Частные производные функций формы
std::array<Eigen::RowVector<double, 8>, 3> LQube::Shape_Func_PD(const double xi, const double eta, const double zeta) const
{
    /*Инициализация*/
    std::array<Eigen::RowVector<double, 8>, 3> dN;
    Eigen::RowVector<double, 8> d_xi, d_eta, d_zeta;

    d_xi[0] = (1/8.0) * (1 - eta) * (-1) * (1-zeta);
    d_xi[1] = (1/8.0) * (1 - eta) * (+1) * (1-zeta);
    d_xi[2] = (1/8.0) * (1 + eta) * (+1) * (1-zeta);
    d_xi[3] = (1/8.0) * (1 + eta) * (-1) * (1-zeta);
    d_xi[4] = (1/8.0) * (1 - eta) * (-1) * (1+zeta);
    d_xi[5] = (1/8.0) * (1 - eta) * (+1) * (1+zeta);
    d_xi[6] = (1/8.0) * (1 + eta) * (+1) * (1+zeta);
    d_xi[7] = (1/8.0) * (1 + eta) * (-1) * (1+zeta);

    d_eta[0] = (1/8.0) * (-1) * (1 - xi) * (1-zeta);
    d_eta[1] = (1/8.0) * (-1) * (1 + xi) * (1-zeta);
    d_eta[2] = (1/8.0) * (+1) * (1 + xi) * (1-zeta);
    d_eta[3] = (1/8.0) * (+1) * (1 - xi) * (1-zeta);
    d_eta[4] = (1/8.0) * (-1) * (1 - xi) * (1+zeta);
    d_eta[5] = (1/8.0) * (-1) * (1 + xi) * (1+zeta);
    d_eta[6] = (1/8.0) * (+1) * (1 + xi) * (1+zeta);
    d_eta[7] = (1/8.0) * (+1) * (1 - xi) * (1+zeta);
    
    d_zeta[0] = (1/8.0) * (1 - eta) * (1 - xi) * (-1);
    d_zeta[1] = (1/8.0) * (1 - eta) * (1 + xi) * (-1);
    d_zeta[2] = (1/8.0) * (1 + eta) * (1 + xi) * (-1);
    d_zeta[3] = (1/8.0) * (1 + eta) * (1 - xi) * (-1);
    d_zeta[4] = (1/8.0) * (1 - eta) * (1 - xi) * (+1);
    d_zeta[5] = (1/8.0) * (1 - eta) * (1 + xi) * (+1);
    d_zeta[6] = (1/8.0) * (1 + eta) * (1 + xi) * (+1);
    d_zeta[7] = (1/8.0) * (1 + eta) * (1 - xi) * (+1);
    
    return {d_xi, d_eta, d_zeta};
}

// Матрица градиентов
Eigen::Matrix<double, 3, 8> LQube::Grad_Mat(const double xi, const double eta, const double zeta) const
{
    /*Инициализация*/
    Eigen::Matrix<double, 3, 8> B;
    std::array<Eigen::RowVector<double, 8>, 3> dN = Shape_Func_PD(xi, eta, zeta);

    B.row(0) = dN[0];
    B.row(1) = dN[1];
    B.row(2) = dN[2];

    return B;
}

// Функция отображения
Point LQube::Mapping(const double xi, const double eta, const double zeta) const
{
    /*Инициализация*/
    double x = 0.0, y = 0.0, z = 0.0;
    Eigen::RowVector<double, 8> N = Shape_Func(xi, eta, zeta);

    for (int i = 0; i < 8; ++i)
    {
        x += N[i] * _coords[3 * i];
        y += N[i] * _coords[3 * i + 1];
        z += N[i] * _coords[3 * i + 2];
    }

    return Point(x, y, z);
}

// Якобиан преобразования
Eigen::Matrix3d LQube::Jacobian(const double xi, const double eta, const double zeta) const
{
    /*Инициализация*/
    Eigen::Matrix3d J;
    std::array<Eigen::RowVector<double, 8>, 3> dN = Shape_Func_PD(xi, eta, zeta);
    Eigen::Matrix<double, 8, 1> X, Y, Z;

    /*Получение векторов-координат по соответствующим осям*/
    for (int i = 0; i < 8; ++i)
    {
        X(i) = _coords[3 * i];
        Y(i) = _coords[3 * i + 1];
        Z(i) = _coords[3 * i + 2];
    }

    /*Заполнение Якобиана преобразования*/
    for (int i = 0; i < 3; ++i)
    {
        J(0, i) = dN[i] * X;
        J(1, i) = dN[i] * Y;
        J(2, i) = dN[i] * Z;
    }

    return J;
}