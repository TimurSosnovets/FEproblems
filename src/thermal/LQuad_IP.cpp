#include  "../../headers/thermal/LQuad_IP.hpp"

// Точки интегрирования
std::array<std::pair<double, double>, 2> IntP = {{ {1/sqrt(3), 1.0}, {-1/sqrt(3), 1.0} }};

// Функции формы
Eigen::RowVector<double, 4> LQuad::Shape_Func(const double xi, const double eta) const
{
    Eigen::RowVector<double, 4> N;
    N[0] = (1/4.0) * (1 - eta) * (1 - xi);
    N[1] = (1/4.0) * (1 - eta) * (1 + xi);
    N[2] = (1/4.0) * (1 + eta) * (1 + xi);
    N[3] = (1/4.0) * (1 + eta) * (1 - xi);
    return N;
};

// Частные производные функций формы 
std::pair<std::array<double, 4>, std::array<double, 4>> LQuad::PD_Shape_Func(const double xi, const double eta) const
{
    std::array<double, 4> d_xi, d_eta;

    d_xi[0] = -(1/4.0) * (1 - eta);
    d_xi[1] = (1/4.0) * (1 - eta);
    d_xi[2] = (1/4.0) * (1 + eta);
    d_xi[3] = -(1/4.0) * (1 + eta);

    d_eta[0] = -(1/4.0) * (1 - xi);
    d_eta[1] = -(1/4.0) * (1 + xi);
    d_eta[2] = (1/4.0) * (1 + xi);
    d_eta[3] = (1/4.0) * (1 - xi);

    return std::make_pair(d_xi, d_eta);
};

// Матрица градиентов
Eigen::Matrix<double, 2, 4> LQuad::Grad_Mat(const double xi, const double eta) const
{
    Eigen::Matrix<double, 2, 4> S;
    S << -(1/4.0) * (1 - eta), (1/4.0) * (1 - eta), (1/4.0) * (1 + eta), -(1/4.0) * (1 + eta),
         -(1/4.0) * (1 - xi),  -(1/4.0) * (1 + xi), (1/4.0) * (1 + xi),  (1/4.0) * (1 - xi);
    return S;
};

// Функция отображения
Point LQuad::Mapping(const double xi, const double eta) const
{
    double x=0, y=0;
    auto N = Shape_Func(xi, eta);
    for (int i=0; i<4; ++i) 
    {
        x += N[i] * _coords[2*i];
        y += N[i] * _coords[2*i + 1];
    }
    return Point(x, y);
    
}

// Якобиан преобразования
Eigen::Matrix2d LQuad::Jacobian(const double xi, const double eta) const
{
    auto dN = PD_Shape_Func(xi, eta);
    Eigen::Matrix2d J = Eigen::Matrix2d::Zero();
    for (int i=0; i<4; ++i)
    {
        J(0, 0) += dN.first[i] * _coords[2 * i];        // ∂x/∂ξ
        J(0, 1) += dN.first[i] * _coords[2 * i + 1];    // ∂y/∂ξ
        J(1, 0) += dN.second[i] * _coords[2 * i];       // ∂x/∂η
        J(1, 1) += dN.second[i] * _coords[2 * i + 1];   // ∂y/∂η
    }
    return J;
}


//Конструктор класcа LQuad
LQuad::LQuad(const std::vector<Node*> v, const Material m) : _material(m)
{   
    if (!(v.size() == 4)) throw std::invalid_argument("LQuad must have exactly 4 vertices!");
    // Создаем вектор координат
    for (int i = 0; i < v.size(); i+=1)
    {
        _coords[2*i] = v[i]->point.x;
        _coords[2*i + 1] = v[i]->point.y; 
    }     
};

// Температура в точке элемента при заданных узловых температурах
const double LQuad::Temperature(const double xi, const double eta, Eigen::Vector<double, 4> nodal_temps) const
{
    Eigen::RowVector<double, 4> N = Shape_Func(xi, eta);
    return (N * nodal_temps);
}

// Репрезентативная температура элемента
const double LQuad::Element_Temp(Eigen::Vector<double, 4> nodal_temps) const
{   
    double T_rep = 0;
    double T;
    
    // Численное интегрирование
    for (int i = 0; i < IntP.size(); ++i)
    {
        for (int j = 0; j < IntP.size(); ++j)
        {   
            T = Temperature(IntP[i].first, IntP[j].first, nodal_temps); // Температура в точках интегрирования
            T_rep += IntP[i].second * IntP[j].second * T;
        }
    }

    return T_rep / 4;
}

// Матрица теплопроводности при заданных узловых температурах
Eigen::Matrix<double, 4, 4> LQuad::Cond_Mat(Eigen::Vector<double, 4> nodal_temps) const
{
    // Вычисление репрезентативной температуры элемента
    const double T_rep = Element_Temp(nodal_temps);

    //Матрица D
    Eigen::Matrix2d D;
    D << _material.get_TCC(T_rep), 0,
          0, _material.get_TCC(T_rep);

    Eigen::Matrix<double, 2, 4> S; // Матрица градиентов
    Eigen::Matrix2d J; // Якобиан преобразования

    Eigen::Matrix<double, 4, 4> H = Eigen::Matrix<double, 4, 4>::Zero();
    
    // Численное интегрирование
    for (int i = 0; i < IntP.size(); ++i)
    {
        for (int j = 0; j < IntP.size(); ++j)
        {   
            S = Grad_Mat(IntP[i].first, IntP[j].first);
            J = Jacobian(IntP[i].first, IntP[j].first);
            const double detJ = J.determinant();
            H += IntP[i].second * IntP[j].second * S.transpose() * J.inverse().transpose() * D * J.inverse() * S * J.determinant();
        }
    }
    return H;
}

// Матрица демпфирования при заданных узловых температурах
Eigen::Matrix<double, 4, 4> LQuad::Damp_Mat(Eigen::Vector<double, 4> nodal_temps) const
{
    // Вычисление репрезентативной температуры элемента
    const double T_rep = Element_Temp(nodal_temps);

    Eigen::RowVector<double, 4> N; // Матрица функций форм
    Eigen::Matrix2d J; // Якобиан преобразования
    Eigen::Matrix<double, 4, 4> C = Eigen::Matrix<double, 4, 4>::Zero();
    
    // Численное интегрирование
    for (int i = 0; i < IntP.size(); ++i)
    {
        for (int j = 0; j < IntP.size(); ++j)
        {   
            N = Shape_Func(IntP[i].first, IntP[j].first);
            J = Jacobian(IntP[i].first, IntP[j].first);
            const double detJ = J.determinant();
            C += IntP[i].second * IntP[j].second * _material.dens() * _material.get_SHC(T_rep) * N.transpose() * N * J.determinant();
        }
    }
    return C; 
}

// Вектор тепловых нагрузок
Eigen::Vector<double, 4> LQuad::Heat_Load_Surf(const double heat_flux, const double eps, Eigen::Vector<double, 4> nodal_temps, std::array<int, 2> s) const
{   
    const double sigma = 5.67e-8; // Постоянная Стефана-Больцмана
    // Выделение температуры поверхности
    const double T_rep = 1.0/2.0 * (nodal_temps[s[0]] + nodal_temps[s[1]]);
    const double J = 1.0 / 2.0 * (Point::distance_to(Point(_coords[2*s[0]], _coords[2*s[0]+1]), Point(_coords[2*s[1]], _coords[2*s[1]+1]))); // Якобиан
    Eigen::RowVector<double, 4> N; // Вектор функций форм
    Eigen::Vector<double, 4> F = Eigen::Vector<double, 4>::Zero(); // Вектор узловых тепловых нагрузок
    // Интегрирование в квадратурах
    for (const auto& w : IntP)
    {
        N = Shape_Func(w.first, -1);
        F += w.second * (heat_flux - eps * sigma * pow(T_rep, 4)) * N.transpose() * J;
    }

    return F;
}