#include "LQube_IP.hpp"

// Точки интегрирования и весовые коэффициенты
std::array<std::pair<double, double>, 2> int_pnts = {{ {1/sqrt(3), 1.0}, {-1/sqrt(3), 1.0} }};
std::array<std::pair<double, double>, 4> heat_int_pnts = {{ {0.861136312, 0.347854845}, {-0.861136312, 0.347854845}, {0.339981044, 0.652145155}, {-0.339981044, 0.652145155} }};
// std::array<std::pair<double, double>, 2> heat_int_pnts = {{ {1/sqrt(3), 1.0}, {-1/sqrt(3), 1.0} }};
// Функции формы
Eigen::RowVectorXd LQube::Shape_Func(const double xi, const double eta, const double zeta) const
{
    /*Инициализация*/
    Eigen::RowVectorXd N(8);

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
std::array<Eigen::RowVectorXd, 3> LQube::Shape_Func_PD(const double xi, const double eta, const double zeta) const
{
    /*Инициализация*/
    std::array<Eigen::RowVectorXd, 3> dN;
    Eigen::RowVectorXd d_xi(8), d_eta(8), d_zeta(8);

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
Eigen::MatrixXd LQube::Grad_Mat(const double xi, const double eta, const double zeta) const
{
    /*Инициализация*/
    Eigen::MatrixXd B(3,8);
    std::array<Eigen::RowVectorXd, 3> dN = Shape_Func_PD(xi, eta, zeta);

    B.row(0) = dN[0];
    B.row(1) = dN[1];
    B.row(2) = dN[2];

    return B;
}

// Функция отображения
Point LQube::Mapping(const double xi, const double eta, const double zeta, const Element& FE) const
{
    if (FE.vertices.size() != 8) {throw std::invalid_argument("8 nodes exactly LQube must have...");}
    /*Инициализация*/
    double x = 0.0, y = 0.0, z = 0.0;
    Eigen::RowVectorXd N = Shape_Func(xi, eta, zeta);

    for (int i = 0; i < 8; ++i)
    {
        x += N[i] * FE.vertices[i]->point.x;
        y += N[i] * FE.vertices[i]->point.y;
        z += N[i] * FE.vertices[i]->point.z;
    }

    return Point(x, y, z);
}

// Якобиан преобразования
Eigen::Matrix3d LQube::Jacobian(const double xi, const double eta, const double zeta, const Element& FE) const
{
    if (FE.vertices.size() != 8) {throw std::invalid_argument("8 nodes exactly LQube must have...");}
    /*Инициализация*/
    Eigen::Matrix3d J, A;
    std::array<Eigen::RowVectorXd, 3> dN = Shape_Func_PD(xi, eta, zeta);
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
    A = J.transpose();
    return A;
}

// Температура в точке элемента при заданных узловых температурах
double LQube::Point_Temp(const double xi, const double eta, const double zeta, const Eigen::VectorXd& nodal_temps) const
{
    Eigen::RowVector<double, 8> N = LQube::Shape_Func(xi, eta, zeta);
    return (N * nodal_temps);
}

// Репрезентативная температура элемента
double LQube::Element_Temp(const Eigen::VectorXd& nodal_temps) const
{
    /*Инициализация*/
    double T_rep = 0; 
    double T;

    // /*Численное интегрирование*/
    // for (int i = 0; i < int_pnts.size(); ++i)
    // {
    //     for (int j = 0; j < int_pnts.size(); ++j)
    //     {   
    //         for (int k = 0; k < int_pnts.size(); ++k)
    //         {
    //             T = Point_Temp(int_pnts[i].first, int_pnts[j].first, int_pnts[k].first, nodal_temps); // Температура в точках интегрирования
    //             T_rep += int_pnts[i].second * int_pnts[j].second * int_pnts[k].second * T;
    //         }
    //     }
    // }

    for (const auto temp : nodal_temps) {T_rep += temp;}

    return T_rep / 8;
}

// Матрица теплопроводности при заданных узловых температурах
Eigen::MatrixXd LQube::Cond_Mat(const Element& FE, const Eigen::VectorXd& nodal_temps) const
{   
    if (FE.vertices.size() != 8) {throw std::invalid_argument("8 nodes exactly LQube must have...");}

    /*Если есть кэш*/
    if (FE.has_cache())
    {
        const double T_rep = Element_Temp(nodal_temps);
        double Lambda = FE.material->get_TCC(T_rep);
        return Lambda * FE.cache.B;
    }

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
                J = Jacobian(int_pnts[i].first, int_pnts[j].first, int_pnts[k].first, FE);
                B = J.inverse() * Grad_Mat(int_pnts[i].first, int_pnts[j].first, int_pnts[k].first);
                B_T = B.transpose();
                detJ = J.determinant();

                H += int_pnts[i].second * int_pnts[j].second * int_pnts[k].second * B_T * Lambda * B * detJ; 
            }
        }
    }

    return H;
} 

// Матрица демфпирования (теплоёмкости) при заданных узловых температурах
Eigen::MatrixXd LQube::Damp_Mat(const Element& FE, const Eigen::VectorXd& nodal_temps) const
{
    if (FE.vertices.size() != 8) {throw std::invalid_argument("8 nodes exactly LQube must have...");}

    /*Если есть кэш*/
    if (FE.has_cache())
    {
        const double T_rep = Element_Temp(nodal_temps);
        double c = FE.material->get_SHC(T_rep);
        double rho = FE.material->dens();
        return c * rho * FE.cache.C;
    }

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
    c = FE.material->get_SHC(T_rep);

    /*Численное интегрирование*/
    for (int i = 0; i < int_pnts.size(); ++i)
    {
        for (int j = 0; j < int_pnts.size(); ++j)
        {   
            for (int k = 0; k < int_pnts.size(); ++k)
            {
                N = Shape_Func(int_pnts[i].first, int_pnts[j].first, int_pnts[k].first);
                N_T = N.transpose();
                J = Jacobian(int_pnts[i].first, int_pnts[j].first, int_pnts[k].first, FE);
                detJ = J.determinant();

                C += int_pnts[i].second * int_pnts[j].second * int_pnts[k].second * rho * c * N_T * N * detJ; 
            }
        }
    }

    return C;
} 

// Вектор узловых нагрузок (с учётом излучения и кривизны поверхности)
Eigen::VectorXd LQube::Heat_Load_Surf(const Element& FE, const double heat_flux, const float eps, const Eigen::VectorXd& nodal_temps) const
{
    if (FE.vertices.size() != 8) {throw std::invalid_argument("8 nodes exactly LQube must have...");}

    /*Инициализация*/
    const float sigma = 5.67e-8; // Постоянная Стефана-Больцмана
    double T_surf = 0.0; // Температура излучающей поверхности
    Eigen::RowVector<double, 8> N_T; // Матрица функций форм (транспонированная)
    Eigen::Vector<double, 8> F = Eigen::Vector<double, 8>::Zero(); // Вектор узловых нагрузок [Вт]
    int surf = 0; // Счётчик
    double J_surf;
    if (FE.is_surface) 
    {
    // Compute edge vectors of the triangular face
    Eigen::Vector3d edge1, edge2;
    edge1 << FE.vertices[1]->point.x - FE.vertices[0]->point.x,
             FE.vertices[1]->point.y - FE.vertices[0]->point.y,
             FE.vertices[1]->point.z - FE.vertices[0]->point.z;

    edge2 << FE.vertices[3]->point.x - FE.vertices[0]->point.x,
             FE.vertices[3]->point.y - FE.vertices[0]->point.y,
             FE.vertices[3]->point.z - FE.vertices[0]->point.z;

    // Compute the surface Jacobian as the norm of the cross product of the edge vectors
    J_surf = edge1.cross(edge2).norm();
    } else {
    // If not a surface, set the surface Jacobian to zero
    J_surf = 0;
    }

    /*Численное интегрирование (по поверхности элемента -> z = -1)*/
    for (int i = 0; i < heat_int_pnts.size(); ++i)
    {
        for (int j = 0; j < heat_int_pnts.size(); ++j)
        {   
            if (FE.has_cache())
            {
                N_T = FE.cache.SF_s[surf];
                ++surf;
            }
            else
            {
                N_T = Shape_Func(heat_int_pnts[i].first, heat_int_pnts[j].first, -1.0).transpose();
            }

            F += (1.0/4.0) * heat_int_pnts[i].second * heat_int_pnts[j].second * (heat_flux) * N_T * FE.surface_area * J_surf; 
        }
    }

    return F;
}

// Нагрузки при спуске
Eigen::VectorXd LQube::Ball_heat_load(const Element& FE, const Geometry& geom, const float eps, const double vel, const double dens, const double Kn, const Eigen::VectorXd& nodal_temps) const
{
    if (FE.vertices.size() != 8) {throw std::invalid_argument("8 nodes exactly LQube must have...");}
    if (!FE.is_surface) { return Eigen::Vector<double, 8>::Zero(); }

    /*Инициализация*/
    const float sigma = 5.67e-8; // Постоянная Стефана-Больцмана
    double T_surf = 0.0, T_env = 300.0; // Температура излучающей поверхности
    Eigen::RowVector<double, 8> N_T; // Матрица функций форм (транспонированная)
    Eigen::Vector<double, 8> F = Eigen::Vector<double, 8>::Zero(); // Вектор узловых нагрузок [Вт]
    int surf = 0; // Счётчик
    double J_surf;
    // Для формирования нагрузки
    double heat_flux; // Плотность теплового потока в точке на поверхности
    double x, y, z;
    double angle;
    // Compute edge vectors of the triangular face
    Eigen::Vector3d edge1, edge2;
    edge1 << FE.vertices[1]->point.x - FE.vertices[0]->point.x,
             FE.vertices[1]->point.y - FE.vertices[0]->point.y,
             FE.vertices[1]->point.z - FE.vertices[0]->point.z;

    edge2 << FE.vertices[3]->point.x - FE.vertices[0]->point.x,
             FE.vertices[3]->point.y - FE.vertices[0]->point.y,
             FE.vertices[3]->point.z - FE.vertices[0]->point.z;

    // Compute the surface Jacobian as the norm of the cross product of the edge vectors
    J_surf = edge1.cross(edge2).norm();

    /*Численное интегрирование (по поверхности элемента -> z = -1)*/
    for (int i = 0; i < heat_int_pnts.size(); ++i)
    {
        for (int j = 0; j < heat_int_pnts.size(); ++j)
        {   
            if (FE.has_cache())
            {
                N_T = FE.cache.SF_s[surf];
                ++surf;
            }
            else
            {
                N_T = Shape_Func(heat_int_pnts[i].first, heat_int_pnts[j].first, -1.0).transpose();
            }
            Point integr = Mapping(heat_int_pnts[i].first, heat_int_pnts[j].first, -1.0, FE);
            angle = heat_angle(integr.x, integr.y, integr.z, geom);
            heat_flux = heat_load(vel, dens, Kn, angle) + eps * sigma * pow(T_env, 4.0);
            T_surf = Point_Temp(heat_int_pnts[i].first, heat_int_pnts[j].first, -1, nodal_temps);

            F += (1.0/4.0) * heat_int_pnts[i].second * heat_int_pnts[j].second * (heat_flux - eps * sigma * pow(T_surf, 4.0)) * N_T * FE.surface_area * J_surf; 
        }
    }

    return F;
}

// Предрасчёт характеристик
void LQube::calculate_element(Element& FE) const
{   
    /*Инициализация*/
    FE.cache.B.resize(8, 8);
    FE.cache.C.resize(8, 8);
    FE.cache.SF_s.resize(heat_int_pnts.size() * heat_int_pnts.size());
    int nbr = 0, surf = 0; // Счётчики

    /*Заполнение*/
    for (int i = 0; i < int_pnts.size(); ++i)
    {
        for (int j = 0; j < int_pnts.size(); ++j)
        {   
            for (int k = 0; k < int_pnts.size(); ++k)
            {   
                Eigen::VectorXd N = Shape_Func(int_pnts[i].first, int_pnts[j].first, int_pnts[k].first);
                Eigen::Matrix3d J = Jacobian(int_pnts[i].first, int_pnts[j].first, int_pnts[k].first, FE);
                Eigen::MatrixXd B = J.inverse() * Grad_Mat(int_pnts[i].first, int_pnts[j].first, int_pnts[k].first);
                double dJ = J.determinant(); 

                FE.cache.B += int_pnts[i].second * int_pnts[j].second * int_pnts[k].second * B.transpose() * B * dJ;
                FE.cache.C += int_pnts[i].second * int_pnts[j].second * int_pnts[k].second * N.transpose() * N * dJ;
                ++nbr;
            }
        }
    }

    for (int i = 0; i < heat_int_pnts.size(); ++i)
    {
        for (int j = 0; j < heat_int_pnts.size(); ++j)
        {   
            FE.cache.SF_s[surf] = Shape_Func(heat_int_pnts[i].first, heat_int_pnts[j].first, -1.0).transpose();
            ++surf;
        }
    }
}
