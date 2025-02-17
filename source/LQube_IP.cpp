#include "LQube_IP.hpp"

// Точки интегрирования и весовые коэффициенты
std::array<std::pair<double, double>, 2> int_pnts = {{ {1/sqrt(3), 1.0}, {-1/sqrt(3), 1.0} }};

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

// Конструктор класса LQube
LQube::LQube(std::vector<Node*> v, const Material& m) : _material(m)
{
    /*Проверка количества узлов*/
    if (!(v.size() == 8)) throw std::invalid_argument("8 vertices must have exactly LQube...");

    /*Заполнение вектора координат*/
    for (int i = 0; i < v.size(); ++i)
    {
        _coords[3 * i] = v[i] -> point.x;
        _coords[3 * i + 1] = v[i] -> point.y;
        _coords[3 * i + 2] = v[i] -> point.z;
    }
}

// Температура в точке элемента при заданных узловых температурах
const double LQube::Point_Temp(const double xi, const double eta, const double zeta, const Eigen::Vector<double, 8>& nodal_temps) const
{
    Eigen::RowVector<double, 8> N = Shape_Func(xi, eta, zeta);
    return (N * nodal_temps);
}

// Репрезентативная температура элемента
const double LQube::Element_Temp(const Eigen::Vector<double, 8>& nodal_temps) const
{
    /*Инициализация*/
    double T_rep = 0; 
    double T;

    /*Численное интегрирование*/
    for (int i = 0; i < int_pnts.size(); ++i)
    {
        for (int j = 0; j < int_pnts.size(); ++j)
        {   
            for (int k = 0; k < int_pnts.size(); ++k)
            {
                T = Point_Temp(int_pnts[i].first, int_pnts[j].first, int_pnts[k].first, nodal_temps); // Температура в точках интегрирования
                T_rep += int_pnts[i].second * int_pnts[j].second * int_pnts[k].second * T;
            }
        }
    }

    return T_rep / 8;
}

// Матрица теплопроводности при заданных узловых температурах
Eigen::Matrix<double, 8, 8> LQube::Cond_Mat(Eigen::Vector<double, 8>& nodal_temps) const
{   
    /*Инициализация*/
    Eigen::Matrix<double, 3, 8> B; // Матрица градиентов
    Eigen::Matrix<double, 3, 8> B_T; // Матрица градиентов (транспонированная)
    Eigen::Matrix3d D; // Матрица материала
    Eigen::Matrix3d J; // Якобиан преобразования
    double detJ; // Детерминант Якобиана преобразования
    Eigen::Matrix<double, 8, 8> H = Eigen::Matrix<double, 8, 8>::Zero(); // Матрица теплопроводности
    int nbr = 0;

    /*Определение репрезентативной температуры элемента*/
    const double T_rep = Element_Temp(nodal_temps);

    /*Заполнение матрицы материала D*/
    double Lambda = _material.get_TCC(T_rep); // Коэффициент теплопроводности при заданной температуре элемента
    D << 
        Lambda, 0, 0,
        0, Lambda, 0,
        0, 0, Lambda;
    
    /*Численное интегрирование*/
    for (int i = 0; i < int_pnts.size(); ++i)
    {
        for (int j = 0; j < int_pnts.size(); ++j)
        {   
            for (int k = 0; k < int_pnts.size(); ++k)
            {   
                if ((Grad.has_value()) && (Grad_T.has_value()) && (dJac.has_value()))
                {
                    B = Grad.value()[nbr];
                    B_T = Grad_T.value()[nbr];
                    detJ = dJac.value()[nbr];
                    ++nbr;
                }
                else
                {
                    J = Jacobian(int_pnts[i].first, int_pnts[j].first, int_pnts[k].first);
                    B = J.inverse() * Grad_Mat(int_pnts[i].first, int_pnts[j].first, int_pnts[k].first);
                    B_T = B.transpose();
                    detJ = J.determinant();
                }

                H += int_pnts[i].second * int_pnts[j].second * int_pnts[k].second * B_T * D * B * detJ; 
            }
        }
    }

    return H;
} 

// Матрица демфпирования (теплоёмкости) при заданных узловых температурах
Eigen::Matrix<double, 8, 8> LQube::Damp_Mat(Eigen::Vector<double, 8>& nodal_temps) const
{
    /*Инициализация*/
    Eigen::RowVector<double, 8> N; // Матрица функций форм
    Eigen::Matrix3d J; // Якобиан преобразования
    double detJ; // Детерминант Якобиана преобразования
    double rho = _material.dens(); // Плотность материала
    double c; // Удельная теплоёмкость материала при заданной температуре
    Eigen::Matrix<double, 8, 8> C = Eigen::Matrix<double, 8, 8>::Zero(); // Матрица демпфирования (теплоёмкости)

    /*Определение репрезентативной температуры элемента*/
    const double T_rep = Element_Temp(nodal_temps);

    /*Численное интегрирование*/
    for (int i = 0; i < int_pnts.size(); ++i)
    {
        for (int j = 0; j < int_pnts.size(); ++j)
        {   
            for (int k = 0; k < int_pnts.size(); ++k)
            {
                N = Shape_Func(int_pnts[i].first, int_pnts[j].first, int_pnts[k].first);
                J = Jacobian(int_pnts[i].first, int_pnts[j].first, int_pnts[k].first);
                detJ = J.determinant();
                c = _material.get_SHC(T_rep);

                C += int_pnts[i].second * int_pnts[j].second * int_pnts[k].second * rho * c * N.transpose() * N * detJ; 
            }
        }
    }

    return C;
} 

// Вектор узловых нагрузок (с учётом излучения и кривизны поверхности)
Eigen::Vector<double, 8> LQube::Heat_Load_Surf(const double heat_flux, const double& eps, Eigen::Vector<double, 8>& nodal_temps, double Jacobian) const
{
    /*Инициализация*/
    const double sigma = 5.67e-8; // Постоянная Стефана-Больцмана
    double T_surf = 0; // Температура излучающей поверхности
    Eigen::RowVector<double, 8> N; // Матрица функций форм
    Eigen::Vector<double, 8> F = Eigen::Vector<double, 8>::Zero(); // Вектор узловых нагрузок [Вт]

    /*Определение репрезентативной температуры излучающей поверхности*/ // Поверхность всегда - на (-1) по Z
    for (int i = 0; i < 4; ++i) {T_surf += (1.0 / 4.0) * nodal_temps(i);}

    /*Численное интегрирование (по поверхности элемента -> z = -1)*/
    for (int i = 0; i < int_pnts.size(); ++i)
    {
        for (int j = 0; j < int_pnts.size(); ++j)
        {   
            N = Shape_Func(int_pnts[i].first, int_pnts[j].first, -1);
            F += (1.0/4.0) * int_pnts[i].second * int_pnts[j].second * (heat_flux - eps * sigma * pow(T_surf, 4.0)) * N.transpose() * Jacobian; 
        }
    }

    return F;
}

// Предрасчёт характеристик
void LQube::calculate_element()
{
    /*Инициализация*/
    Grad = std::array<Eigen::Matrix<double, 3, 8>, 8>{};
    Grad_T = std::array<Eigen::Matrix<double, 8, 3>, 8>{};
    Shape = std::array<Eigen::RowVector<double, 8>, 8>{};
    Shape_T = std::array<Eigen::Vector<double, 8>, 8>{};
    dJac = std::array<double, 8>{};
    int nbr = 0;

    /*Заполнение*/
    for (int i = 0; i < int_pnts.size(); ++i)
        {
        for (int j = 0; j < int_pnts.size(); ++j)
        {   
            for (int k = 0; k < int_pnts.size(); ++k)
            {   
                auto N = Shape_Func(int_pnts[i].first, int_pnts[j].first, int_pnts[k].first);
                auto J = Jacobian(int_pnts[i].first, int_pnts[j].first, int_pnts[k].first);
                auto B = J.inverse() * Grad_Mat(int_pnts[i].first, int_pnts[j].first, int_pnts[k].first);
                auto dJ = J.determinant(); 

                Grad.value()[nbr] = B;
                Grad_T.value()[nbr] = B.transpose();
                Shape.value()[nbr] = N;
                Shape_T.value()[nbr] = N.transpose();
                dJac.value()[nbr] = dJ;
                ++nbr;
            }
        }
    }   
}
