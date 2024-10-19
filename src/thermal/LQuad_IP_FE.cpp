#include  "../../headers/thermal/LQuad_IP_FE.hpp"

// Точки интегрирования
std::array<std::pair<double, double>, 2> IntP = {{ {1/sqrt(3), 1.0}, {-1/sqrt(3), 1.0} }};

// Функции формы
Eigen::RowVector<double, 4> LQuad::Shape_Func(const double xi, const double eta) 
{
    Eigen::RowVector<double, 4> N;
    N[0] = (1/4) * (1 - eta) * (1 - xi);
    N[1] = (1/4) * (1 - eta) * (1 + xi);
    N[2] = (1/4) * (1 + eta) * (1 + xi);
    N[3] = (1/4) * (1 + eta) * (1 - xi);
    return N;
};

// Частные производные функций формы 
std::pair<std::array<double, 4>, std::array<double, 4>> LQuad::PD_Shape_Func(const double xi, const double eta) 
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
Eigen::Matrix<double, 2, 4> LQuad::Grad_Mat(const double xi, const double eta)
{
    Eigen::Matrix<double, 2, 4> S;
    S << -(1/4.0) * (1 - eta), (1/4.0) * (1 - eta), (1/4.0) * (1 + eta), -(1/4.0) * (1 + eta),
         -(1/4.0) * (1 - xi),  -(1/4.0) * (1 + xi), (1/4.0) * (1 + xi),  (1/4.0) * (1 - xi);
    return S;
};

// Функция отображения
Point LQuad::Mapping(const double xi, const double eta)
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
Eigen::Matrix2d LQuad::Jacobian(const double xi, const double eta)
{
    auto dN = PD_Shape_Func(xi, eta);
    Eigen::Matrix2d J = Eigen::Matrix2d::Zero();
    for (int i=0; i<4; ++i)
    {
        J(0) += dN.first[i] * _coords[2*i];
        J(1) += dN.first[i] * _coords[2*i + 1];
        J(2) += dN.second[i] * _coords[2*i];
        J(3) += dN.second[i] * _coords[2*i + 1];
    }
    return J;
}

//Конструктор класcа LQuad
LQuad::LQuad(const std::array<std::reference_wrapper<std::pair<Point, int>>, 4> Vertices, const double k1, const double k2) : _verts(Vertices)
{   
    if (Vertices.size() != 4) throw std::invalid_argument("The LQuad FE must have exactly 4 vertices"); 

    _D << k1, 0,
          0, k2;
    
    // Создаем вектор координат
    for (int i = 0; i < Vertices.size(); i+=1)
    {
        _coords[2*i] = Vertices[i].get().first.x;
        _coords[2*i + 1] = Vertices[i].get().first.y; 
    }

    // Заполнение матрицы теплопроводности
    Eigen::Matrix<double, 2, 4> S; // Матрица градиентов
    Eigen::Matrix2d J; // Якобиан преобразования
    _H = Eigen::Matrix<double, 4, 4>::Zero();
    for (int i = 0; i < IntP.size(); ++i)
    {
        for (int j = 0; j < IntP.size(); ++j)
        {   
            S = Grad_Mat(IntP[i].first, IntP[j].first);
            J = Jacobian(IntP[i].first, IntP[j].first);
            _H += IntP[i].second * IntP[j].second * S.transpose() * J.inverse().transpose() * _D * J.inverse() * S * J.determinant();
        }
    }
      
};

// Возвращаемые значения
const std::array<std::reference_wrapper<std::pair<Point, int>>, 4> LQuad::Vertices() const
{
    return _verts;
}

const Eigen::Matrix2d& LQuad::D() const
{
    return _D;
}

const Eigen::Matrix<double, 4, 4>& LQuad::Cond_Mat() const
{
    return _H;
}

// Температура в точке элемента
double LQuad::Temperature(const double xi, const double eta, const double& T1, const double& T2, const double& T3, const double& T4)
{
    Eigen::RowVector<double, 4> N = Shape_Func(xi, eta);
    return (N[0] * T1 + N[1] * T2 + N[2] * T3 + N[3] * T4);
}