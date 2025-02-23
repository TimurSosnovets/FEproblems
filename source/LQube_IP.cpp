#include "LQube_IP.hpp"

// Точки интегрирования и весовые коэффициенты
std::array<std::pair<double, double>, 2> int_pnts = {{ {1/sqrt(3), 1.0}, {-1/sqrt(3), 1.0} }};

// Функции формы
Eigen::RowVector<double, 8> LQube::Shape_Func(const double xi, const double eta, const double zeta)
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
std::array<Eigen::RowVector<double, 8>, 3> LQube::Shape_Func_PD(const double xi, const double eta, const double zeta)
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
Eigen::Matrix<double, 3, 8> LQube::Grad_Mat(const double xi, const double eta, const double zeta)
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
Point LQube::Mapping(const double xi, const double eta, const double zeta, const Element& FE)
{
    if (!FE.vertices.size() == 8) {throw std::invalid_argument("8 nodes exactly LQube must have...");}
    /*Инициализация*/
    double x = 0.0, y = 0.0, z = 0.0;
    Eigen::RowVector<double, 8> N = Shape_Func(xi, eta, zeta);

    for (int i = 0; i < 8; ++i)
    {
        x += N[i] * FE.vertices[i]->point.x;
        y += N[i] * FE.vertices[i]->point.y;
        z += N[i] * FE.vertices[i]->point.z;
    }

    return Point(x, y, z);
}

// Якобиан преобразования
Eigen::Matrix3d LQube::Jacobian(const double xi, const double eta, const double zeta, const Element& FE)
{
    if (!FE.vertices.size() == 8) {throw std::invalid_argument("8 nodes exactly LQube must have...");}
    /*Инициализация*/
    Eigen::Matrix3d J;
    std::array<Eigen::RowVector<double, 8>, 3> dN = Shape_Func_PD(xi, eta, zeta);
    Eigen::Matrix<double, 8, 1> X, Y, Z;

    /*Получение векторов-координат по соответствующим осям*/
    for (int i = 0; i < 8; ++i)
    {
        X(i) = FE.vertices[i]->point.x;
        Y(i) = FE.vertices[i]->point.y;
        Z(i) = FE.vertices[i]->point.z;
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

// Температура в точке элемента при заданных узловых температурах
double LQube::Point_Temp(const double xi, const double eta, const double zeta, const Eigen::Vector<double, 8>& nodal_temps)
{
    Eigen::RowVector<double, 8> N = LQube::Shape_Func(xi, eta, zeta);
    return (N * nodal_temps);
}

// Репрезентативная температура элемента
double LQube::Element_Temp(const Eigen::Vector<double, 8>& nodal_temps)
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
Eigen::Matrix<double, 8, 8> LQube::Cond_Mat(const Element& FE, const Eigen::Vector<double, 8>& nodal_temps)
{   
    if (!FE.vertices.size() == 8) {throw std::invalid_argument("8 nodes exactly LQube must have...");}
    /*Инициализация*/
    Eigen::Matrix<double, 3, 8> B; // Матрица градиентов
    Eigen::Matrix<double, 8, 3> B_T; // Матрица градиентов (транспонированная)
    Eigen::Matrix3d D; // Матрица материала
    Eigen::Matrix3d J; // Якобиан преобразования
    double detJ; // Детерминант Якобиана преобразования
    Eigen::Matrix<double, 8, 8> H = Eigen::Matrix<double, 8, 8>::Zero(); // Матрица теплопроводности
    int nbr = 0; // Счётчик

    /*Определение репрезентативной температуры элемента*/
    const double T_rep = Element_Temp(nodal_temps);

    /*Заполнение матрицы материала D*/
    double Lambda = FE.material->get_TCC(T_rep); // Коэффициент теплопроводности при заданной температуре элемента
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
                    J = Jacobian(int_pnts[i].first, int_pnts[j].first, int_pnts[k].first, FE);
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
Eigen::Matrix<double, 8, 8> LQube::Damp_Mat(const Element& FE, const Eigen::Vector<double, 8>& nodal_temps)
{
    /*Инициализация*/
    Eigen::RowVector<double, 8> N; // Матрица функций форм
    Eigen::Vector<double, 8> N_T; // Матрица функций форм (транспонированная)
    Eigen::Matrix3d J; // Якобиан преобразования
    double detJ; // Детерминант Якобиана преобразования
    double rho = FE.material->dens(); // Плотность материала
    double c; // Удельная теплоёмкость материала при заданной температуре
    Eigen::Matrix<double, 8, 8> C = Eigen::Matrix<double, 8, 8>::Zero(); // Матрица демпфирования (теплоёмкости)
    int nbr = 0; // Счётчик

    /*Определение репрезентативной температуры элемента*/
    const double T_rep = Element_Temp(nodal_temps);

    /*Численное интегрирование*/
    for (int i = 0; i < int_pnts.size(); ++i)
    {
        for (int j = 0; j < int_pnts.size(); ++j)
        {   
            for (int k = 0; k < int_pnts.size(); ++k)
            {
                if ((Shape.has_value()) && (Shape_T.has_value()) && (dJac.has_value()))
                {
                    N = Shape.value()[nbr];
                    N_T = Shape_T.value()[nbr];
                    detJ = dJac.value()[nbr];
                    ++nbr;
                }
                else
                {
                    N = Shape_Func(int_pnts[i].first, int_pnts[j].first, int_pnts[k].first);
                    N_T = N.transpose();
                    J = Jacobian(int_pnts[i].first, int_pnts[j].first, int_pnts[k].first, FE);
                    detJ = J.determinant();
                }
                c = FE.material->get_SHC(T_rep);

                C += int_pnts[i].second * int_pnts[j].second * int_pnts[k].second * rho * c * N_T * N * detJ; 
            }
        }
    }

    return C;
} 

// Вектор узловых нагрузок (с учётом излучения и кривизны поверхности)
Eigen::Vector<double, 8> LQube::Heat_Load_Surf(const Element& FE, const double heat_flux, const float eps, const Eigen::Vector<double, 8>& nodal_temps, const float surf_area)
{
    /*Инициализация*/
    const float sigma = 5.67e-8; // Постоянная Стефана-Больцмана
    double T_surf = 0.0; // Температура излучающей поверхности
    Eigen::RowVector<double, 8> N_T; // Матрица функций форм (транспонированная)
    Eigen::Vector<double, 8> F = Eigen::Vector<double, 8>::Zero(); // Вектор узловых нагрузок [Вт]
    int surf = 0; // Счётчик

    /*Определение репрезентативной температуры излучающей поверхности*/ // Поверхность всегда - на (-1) по Z
    for (int i = 0; i < 4; ++i) {T_surf += (1.0 / 4.0) * nodal_temps(i);}

    /*Численное интегрирование (по поверхности элемента -> z = -1)*/
    for (int i = 0; i < int_pnts.size(); ++i)
    {
        for (int j = 0; j < int_pnts.size(); ++j)
        {   
            if (Shape_surf.has_value())
            {
                N_T = Shape_surf.value()[surf];
                ++surf;
            }
            else
            {
                N_T = Shape_Func(int_pnts[i].first, int_pnts[j].first, -1.0).transpose();
            }

            F += (1.0/4.0) * int_pnts[i].second * int_pnts[j].second * (heat_flux - eps * sigma * pow(T_surf, 4.0)) * N_T * surf_area; 
        }
    }

    return F;
}

// // Предрасчёт характеристик
// void LQube::calculate_element()
// {
//     /*Инициализация*/
//     Grad = std::array<Eigen::Matrix<double, 3, 8>, 8>{};
//     Grad_T = std::array<Eigen::Matrix<double, 8, 3>, 8>{};
//     Shape = std::array<Eigen::RowVector<double, 8>, 8>{};
//     Shape_T = std::array<Eigen::Vector<double, 8>, 8>{};
//     Shape_surf = std::array<Eigen::Vector<double, 8>, 4>{};
//     dJac = std::array<double, 8>{};
//     int nbr = 0, surf = 0; // Счётчики

//     /*Заполнение*/
//     for (int i = 0; i < int_pnts.size(); ++i)
//     {
//         for (int j = 0; j < int_pnts.size(); ++j)
//         {   
//             Shape_surf.value()[surf] = Shape_Func(int_pnts[i].first, int_pnts[j].first, -1.0).transpose();
//             ++surf;
//             for (int k = 0; k < int_pnts.size(); ++k)
//             {   
//                 auto N = Shape_Func(int_pnts[i].first, int_pnts[j].first, int_pnts[k].first);
//                 auto J = Jacobian(int_pnts[i].first, int_pnts[j].first, int_pnts[k].first);
//                 auto B = J.inverse() * Grad_Mat(int_pnts[i].first, int_pnts[j].first, int_pnts[k].first);
//                 auto dJ = J.determinant(); 

//                 Grad.value()[nbr] = B;
//                 Grad_T.value()[nbr] = B.transpose();
//                 Shape.value()[nbr] = N;
//                 Shape_T.value()[nbr] = N.transpose();
//                 dJac.value()[nbr] = dJ;
//                 ++nbr;
//             }
//         }
//     }   
// }
