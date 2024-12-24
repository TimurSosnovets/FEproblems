#include  "../../headers/thermal/LQuad_NEW.hpp"

// Точки интегрирования
std::array<std::pair<double, double>, 2> IntP = {{ {1/sqrt(3), 1.0}, {-1/sqrt(3), 1.0} }};

// Функции формы
Eigen::RowVector<double, 4> NEW_LQuad::Shape_Func(const double xi, const double eta) const
{
    Eigen::RowVector<double, 4> N;
    N[0] = (1/4.0) * (1 - eta) * (1 - xi);
    N[1] = (1/4.0) * (1 - eta) * (1 + xi);
    N[2] = (1/4.0) * (1 + eta) * (1 + xi);
    N[3] = (1/4.0) * (1 + eta) * (1 - xi);
    return N;
};

// Частные производные функций формы 
std::pair<std::array<double, 4>, std::array<double, 4>> NEW_LQuad::PD_Shape_Func(const double xi, const double eta) const
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
Eigen::Matrix<double, 2, 4> NEW_LQuad::Grad_Mat(const double xi, const double eta) const
{
    Eigen::Matrix<double, 2, 4> S;
    S << -(1/4.0) * (1 - eta), (1/4.0) * (1 - eta), (1/4.0) * (1 + eta), -(1/4.0) * (1 + eta),
         -(1/4.0) * (1 - xi),  -(1/4.0) * (1 + xi), (1/4.0) * (1 + xi),  (1/4.0) * (1 - xi);
    return S;
};

// Функция отображения
Point NEW_LQuad::Mapping(const double xi, const double eta) const
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
Eigen::Matrix2d NEW_LQuad::Jacobian(const double xi, const double eta) const
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


//Конструктор класcа NEW_LQuad
NEW_LQuad::NEW_LQuad(const int N, const std::array<Node&, 4> Vertices, const Material M, std::array<double, 4> ND_T) : _Number(N), _Vertices(Vertices), mat(M)
{    
    // Репрезентативная температура (Интерполирование через функции формы)
    for (int i = 0; i < IntP.size(); ++i)
    {
        for (int j = 0; j < IntP.size(); ++j)
        {   
            auto N = Shape_Func(IntP[i].first, IntP[j].first); // Функции формы в точках интегрирования
            T_rep += IntP[i].second * IntP[j].second * (N[0] * ND_T[0] + N[1] * ND_T[1] + N[2] * ND_T[2] * N[3] * ND_T[3]);
        }
    }
    
    // Матрица D
    Eigen::Matrix2d D;
    D << mat.get_TCC(T_rep), 0,
          0, mat.get_TCC(T_rep);
    
    // Создаем вектор координат
    for (int i = 0; i < Vertices.size(); i+=1)
    {
        _coords[2*i] = Vertices[i].pnt.x;
        _coords[2*i + 1] = Vertices[i].pnt.y; 
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
            double detJ = J.determinant();
            _H += IntP[i].second * IntP[j].second * S.transpose() * J.inverse().transpose() * D * J.inverse() * S * J.determinant();
        }
    }

    // Заполнение матрицы демпфирования
    
      
};


// Возвращаемые значения
const const std::array<Node&, 4> NEW_LQuad::Vertices() const
{
    return _Vertices;
}

const Eigen::Matrix<double, 4, 4>& NEW_LQuad::Cond_Mat() const
{
    return _H;
}

// Температура в точке элемента
double NEW_LQuad::Temperature(const double xi, const double eta, const double& T1, const double& T2, const double& T3, const double& T4) const
{
    Eigen::RowVector<double, 4> N = Shape_Func(xi, eta);
    return (N[0] * T1 + N[1] * T2 + N[2] * T3 + N[3] * T4);
}

// Вектор тепловых нагрузок
Eigen::Vector<double, 4> NEW_LQuad::Heat_Load(const double heat_flux) const
{
    Eigen::Matrix2d J; // Якобиан
    Eigen::RowVector<double, 4> N; // Вектор функций форм
    Eigen::Vector<double, 4> F = Eigen::Vector<double, 4>::Zero(); // Вектор узловых тепловых нагрузок
    // Интегрирование в квадратурах
    for (const auto& w : IntP)
    {
        J = Jacobian(w.first, -1);
        std::cout << "\n Jacobian det: \n" << J.determinant() << "\n"; 
        N = Shape_Func(w.first, -1);
        //std::cout << "\n Shape Functions: \n" << N << "\n"; 
        F += w.second * heat_flux * N.transpose() * J(0,0);
    }
    return F;
}