#include "LWedge_IP.hpp"

// Точки интегрирования и весовые коэффициенты
std::array<std::pair<double, double>, 2> linear_int = {{ {1/sqrt(3), 1.0}, {-1/sqrt(3), 1.0} }};
std::array<std::pair<double, double>, 3> triang_int = {{ {1/2.0, 1/2.0}, {1/2.0, 0}, {0, 1/2.0} }};
// std::array<std::pair<double, double>, 7> heat_triang_int = {{ {1.0/3.0, 1.0/3.0}, {1.0/2.0, 1.0/2.0}, {1.0/2.0, 0}, {0, 1.0/2.0}, {1.0, 0.0}, {0.0, 1.0}, {0.0, 0.0} }};
// std::array<double, 7> heat_triang_weigth = {27.0/60.0, 8.0/60.0, 8.0/60.0, 8.0/60.0, 3.0/60.0, 3.0/60.0, 3.0/60.0};
std::array<std::pair<double, double>, 3> heat_triang_int = {{ {1/2.0, 1/2.0}, {1/2.0, 0}, {0, 1/2.0} }};
std::array<double, 3> heat_triang_weigth = {1/3.0, 1/3.0, 1/3.0};

// Функции формы (xi - L1; eta - L2)
Eigen::RowVectorXd LWedge::Shape_Func(const double xi, const double eta, const double zeta) const
{
    /*Инициализация*/
    Eigen::RowVectorXd N(6);

    N[0] = (1/2.0) * (xi) * (1 - zeta);
    N[1] = (1/2.0) * (eta) * (1 - zeta);
    N[2] = (1/2.0) * (1 - xi - eta) * (1 - zeta);
    N[3] = (1/2.0) * (xi) * (1 + zeta);
    N[4] = (1/2.0) * (eta) * (1 + zeta);
    N[5] = (1/2.0) * (1 - xi - eta) * (1 + zeta);

    return N;
}

// Частные производные функций формы
std::array<Eigen::RowVectorXd, 3> LWedge::Shape_Func_PD(const double xi, const double eta, const double zeta) const
{
    /*Инициализация*/
    std::array<Eigen::RowVectorXd, 3> dN;
    Eigen::RowVectorXd d_xi(6), d_eta(6), d_zeta(6);

    d_xi[0] = (1/2.0) * (+1) * (1 - zeta);
    d_xi[1] = (1/2.0) * (+0) * (1 - zeta);
    d_xi[2] = (1/2.0) * (-1) * (1 - zeta);
    d_xi[3] = (1/2.0) * (+1) * (1 + zeta);
    d_xi[4] = (1/2.0) * (+0) * (1 + zeta);
    d_xi[5] = (1/2.0) * (-1) * (1 + zeta);

    d_eta[0] = (1/2.0) * (+0) * (1 - zeta);
    d_eta[1] = (1/2.0) * (+1) * (1 - zeta);
    d_eta[2] = (1/2.0) * (-1) * (1 - zeta);
    d_eta[3] = (1/2.0) * (+0) * (1 + zeta);
    d_eta[4] = (1/2.0) * (+1) * (1 + zeta);
    d_eta[5] = (1/2.0) * (-1) * (1 + zeta);
    
    d_zeta[0] = (1/2.0) * (xi) * (-1);
    d_zeta[1] = (1/2.0) * (eta) * (-1);
    d_zeta[2] = (1/2.0) * (1 - xi - eta) * (-1);
    d_zeta[3] = (1/2.0) * (xi) * (+1);
    d_zeta[4] = (1/2.0) * (eta) * (+1);
    d_zeta[5] = (1/2.0) * (1 - xi - eta) * (+1);
    
    return {d_xi, d_eta, d_zeta};
}

// Матрица градиентов
Eigen::MatrixXd LWedge::Grad_Mat(const double xi, const double eta, const double zeta) const
{
    /*Инициализация*/
    Eigen::MatrixXd B(3,6);
    std::array<Eigen::RowVectorXd, 3> dN = Shape_Func_PD(xi, eta, zeta);

    B.row(0) = dN[0];
    B.row(1) = dN[1];
    B.row(2) = dN[2];

    return B;
}

// Функция отображения
Point LWedge::Mapping(const double xi, const double eta, const double zeta, const Element& FE) const
{
    if (FE.vertices.size() != 6) {throw std::invalid_argument("6 nodes exactly LWedge must have...");}
    /*Инициализация*/
    double x = 0.0, y = 0.0, z = 0.0;
    Eigen::RowVectorXd N = Shape_Func(xi, eta, zeta);

    for (int i = 0; i < 6; ++i)
    {
        x += N[i] * FE.vertices[i]->point.x;
        y += N[i] * FE.vertices[i]->point.y;
        z += N[i] * FE.vertices[i]->point.z;
    }

    return Point(x, y, z);
}

// Якобиан преобразования
Eigen::Matrix3d LWedge::Jacobian(const double xi, const double eta, const double zeta, const Element& FE) const
{
    if (FE.vertices.size() != 6) {throw std::invalid_argument("6 nodes exactly LWedge must have...");}
    /*Инициализация*/
    Eigen::Matrix3d J, A;
    std::array<Eigen::RowVectorXd, 3> dN = Shape_Func_PD(xi, eta, zeta);
    Eigen::Matrix<double, 6, 1> X, Y, Z;

    /*Получение векторов-координат по соответствующим осям*/
    for (int i = 0; i < 6; ++i)
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
double LWedge::Point_Temp(const double xi, const double eta, const double zeta, const Eigen::VectorXd& nodal_temps) const
{
    Eigen::RowVector<double, 6> N = LWedge::Shape_Func(xi, eta, zeta);
    return (N * nodal_temps);
}

// Репрезентативная температура элемента
double LWedge::Element_Temp(const Eigen::VectorXd& nodal_temps) const
{
    /*Инициализация*/
    double T_rep = 0; 
    double T;

    // /*Численное интегрирование*/

    // for (int i = 0; i < linear_int.size(); ++i)
    // {
    //     for (int t = 0; t < triang_int.size(); ++t)
    //     {
    //         T = Point_Temp(triang_int[t].first, triang_int[t].second, linear_int[i].first, nodal_temps); // Температура в точках интегрирования
    //         T_rep += linear_int[i].second * 1.0/6.0 * T;
    //     }
    // }
    for (const auto temp : nodal_temps) {T_rep += temp;}
    return T_rep / 6;
}

// Матрица теплопроводности при заданных узловых температурах
Eigen::MatrixXd LWedge::Cond_Mat(const Element& FE, const Eigen::VectorXd& nodal_temps) const
{   
    if (FE.vertices.size() != 6) {throw std::invalid_argument("6 nodes exactly LWedge must have...");}

    /*Если есть кэш*/
    if (FE.has_cache())
    {
        const double T_rep = Element_Temp(nodal_temps);
        double Lambda = FE.material->get_TCC(T_rep);
        return Lambda * FE.cache.B;
    }

    /*Инициализация*/
    Eigen::Matrix<double, 3, 6> B; // Матрица градиентов
    Eigen::Matrix<double, 6, 3> B_T; // Матрица градиентов (транспонированная)
    Eigen::Matrix3d D; // Матрица материала
    Eigen::Matrix3d J; // Якобиан преобразования
    double detJ; // Детерминант Якобиана преобразования
    Eigen::Matrix<double, 6, 6> H = Eigen::Matrix<double, 6, 6>::Zero(); // Матрица теплопроводности
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
    for (int t = 0; t < triang_int.size(); ++t)
    {
        for (int i = 0; i < linear_int.size(); ++i)
        {
            J = Jacobian(triang_int[t].first, triang_int[t].second, linear_int[i].first, FE);
            B = J.inverse() * Grad_Mat(triang_int[t].first, triang_int[t].second, linear_int[i].first);
            B_T = B.transpose();
            detJ = J.determinant();
            
            H += linear_int[i].second * 1.0/6.0 * B_T * D * B * detJ;
        }
    } 

    return H;
} 

// Матрица демфпирования (теплоёмкости) при заданных узловых температурах
Eigen::MatrixXd LWedge::Damp_Mat(const Element& FE, const Eigen::VectorXd& nodal_temps) const
{
    if (FE.vertices.size() != 6) {throw std::invalid_argument("6 nodes exactly LWedge must have...");}

    /*Если есть кэш*/
    if (FE.has_cache())
    {
        const double T_rep = Element_Temp(nodal_temps);
        double c = FE.material->get_SHC(T_rep);
        double rho = FE.material->dens();
        return c * rho * FE.cache.C;
    }

    /*Инициализация*/
    Eigen::RowVector<double, 6> N; // Матрица функций форм
    Eigen::Vector<double, 6> N_T; // Матрица функций форм (транспонированная)
    Eigen::Matrix3d J; // Якобиан преобразования
    double detJ; // Детерминант Якобиана преобразования
    double rho = FE.material->dens(); // Плотность материала
    double c; // Удельная теплоёмкость материала при заданной температуре
    Eigen::Matrix<double, 6, 6> C = Eigen::Matrix<double, 6, 6>::Zero(); // Матрица демпфирования (теплоёмкости)
    int nbr = 0; // Счётчик

    /*Определение репрезентативной температуры элемента*/
    const double T_rep = Element_Temp(nodal_temps);
    c = FE.material->get_SHC(T_rep);

    /*Численное интегрирование*/
    for (int t = 0; t < triang_int.size(); ++t)
    {
        for (int i = 0; i < linear_int.size(); ++i)
        {
            N = Shape_Func(triang_int[t].first, triang_int[t].second, linear_int[i].first);
            N_T = N.transpose();
            J = Jacobian(triang_int[t].first, triang_int[t].second, linear_int[i].first, FE);
            detJ = J.determinant();

            C += linear_int[i].second * 1.0/6.0 * rho * c * N_T * N * detJ;
        }
    }

    return C;
} 

// Вектор узловых нагрузок (с учётом излучения и БЕЗ УЧЁТА кривизны поверхности)
Eigen::VectorXd LWedge::Heat_Load_Surf(const Element& FE, const double heat_flux, const float eps, const Eigen::VectorXd& nodal_temps) const
{
    if (FE.vertices.size() != 6) {throw std::invalid_argument("6 nodes exactly LWedge must have...");}

    /*Инициализация*/
    const float sigma = 5.67e-8; // Постоянная Стефана-Больцмана
    double T_surf; // Температура излучающей поверхности
    double J_surf;
    if (FE.is_surface) 
    {
    // Compute edge vectors of the triangular face
    Eigen::Vector3d edge1, edge2;
    edge1 << FE.vertices[1]->point.x - FE.vertices[0]->point.x,
             FE.vertices[1]->point.y - FE.vertices[0]->point.y,
             FE.vertices[1]->point.z - FE.vertices[0]->point.z;

    edge2 << FE.vertices[2]->point.x - FE.vertices[0]->point.x,
             FE.vertices[2]->point.y - FE.vertices[0]->point.y,
             FE.vertices[2]->point.z - FE.vertices[0]->point.z;

    T_surf = 1.0/3.0 * (nodal_temps[0] + nodal_temps[1] + nodal_temps[2]);
    // Compute the surface Jacobian as the norm of the cross product of the edge vectors
    J_surf = edge1.cross(edge2).norm();
    } else {
    // If not a surface, set the surface Jacobian to zero
    J_surf = 0;
    }
    Eigen::RowVector<double, 6> N_T; // Матрица функций форм (транспонированная)
    Eigen::Vector<double, 6> F = Eigen::Vector<double, 6>::Zero(); // Вектор узловых нагрузок [Вт]
    int surf = 0; // Счётчик

    /*Определение репрезентативной температуры излучающей поверхности*/ // Поверхность всегда - на (-1) по Z
    for (int i = 0; i < 3; ++i) {T_surf += (1.0 / 3.0) * nodal_temps(i);}

    /*Численное интегрирование (по поверхности элемента -> z = -1)*/
    for (int t = 0; t < triang_int.size(); ++t)
    {      
        if (FE.has_cache())
        {
            N_T = FE.cache.SF_s[surf];
            ++surf;
        }
        else
        {
            N_T = Shape_Func(triang_int[t].first, triang_int[t].second, -1.0).transpose();
        }
        F += (1.0/6.0) * (heat_flux - eps * sigma * pow(T_surf, 4.0)) * N_T * J_surf;
    }

    return F;
}

Eigen::VectorXd LWedge::Ball_heat_load(const Element& FE, const Geometry& geom, const float eps, const double vel, const double dens, const double Kn, const Eigen::VectorXd& nodal_temps) const
{
    if (FE.vertices.size() != 6) {throw std::invalid_argument("6 nodes exactly LWedge must have...");}
    if (!FE.is_surface) { return Eigen::Vector<double, 6>::Zero(); }

    /*Инициализация*/
    const float sigma = 5.67e-8; // Постоянная Стефана-Больцмана
    double T_surf, T_env = 300.0; // Температура излучающей поверхности
    double J_surf;
    int surf = 0; // Счётчик
    Eigen::RowVector<double, 6> N_T; // Матрица функций форм (транспонированная)
    Eigen::Vector<double, 6> F = Eigen::Vector<double, 6>::Zero(); // Вектор узловых нагрузок [Вт]

    // Для формирования нагрузки
    double heat_flux; // Плотность теплового потока в точке на поверхности
    double x, y, z;
    double angle;

    // Compute edge vectors of the triangular face
    Eigen::Vector3d edge1, edge2;
    edge1 << FE.vertices[1]->point.x - FE.vertices[0]->point.x,
             FE.vertices[1]->point.y - FE.vertices[0]->point.y,
             FE.vertices[1]->point.z - FE.vertices[0]->point.z;

    edge2 << FE.vertices[2]->point.x - FE.vertices[0]->point.x,
             FE.vertices[2]->point.y - FE.vertices[0]->point.y,
             FE.vertices[2]->point.z - FE.vertices[0]->point.z;

    // Compute the surface Jacobian as the norm of the cross product of the edge vectors
    J_surf = edge1.cross(edge2).norm();

    /*Численное интегрирование (по поверхности элемента -> z = -1)*/
    for (int t = 0; t < heat_triang_int.size(); ++t)
    {      
        if (FE.has_cache())
        {
            N_T = FE.cache.SF_s[surf];
            ++surf;
        }
        else
        {
            N_T = Shape_Func(heat_triang_int[t].first, heat_triang_int[t].second, -1.0).transpose();
        }
        Point integr = Mapping(heat_triang_int[t].first, heat_triang_int[t].second, -1.0, FE);
        angle = heat_angle(integr.x, integr.y, integr.z, geom);
        // heat_flux = heat_load(vel, dens, Kn, angle) + eps * sigma * pow(T_env, 4.0);
        // T_surf = Point_Temp(heat_triang_int[t].first, heat_triang_int[t].second, -1, nodal_temps);

        // F += heat_triang_weigth[t] * (1.0/2.0) * (heat_flux - eps * sigma * pow(T_surf, 4.0)) * N_T * J_surf;

        heat_flux = heat_load(vel, dens, Kn, angle);
        heat_flux = 1e5;

        F += heat_triang_weigth[t] * (1.0/2.0) * (heat_flux) * N_T * abs(J_surf);
    }
    
    return F;
}

// Предрасчёт характеристик
void LWedge::calculate_element(Element& FE) const
{   
    /*Инициализация*/
    FE.cache.B.resize(6, 6);
    FE.cache.C.resize(6, 6);
    FE.cache.SF_s.resize(7);
    int nbr = 0, surf = 0; // Счётчики

    /*Заполнение*/
    for (int t = 0; t < triang_int.size(); ++t)
    {        
        for (int i = 0; i < linear_int.size(); ++i)
        {
            Eigen::VectorXd N = Shape_Func(triang_int[t].first, triang_int[t].second, linear_int[i].first);
            Eigen::Matrix3d J = Jacobian(triang_int[t].first, triang_int[t].second, linear_int[i].first, FE);
            Eigen::MatrixXd B = J.inverse() * Grad_Mat(triang_int[t].first, triang_int[t].second, linear_int[i].first);
            double dJ = J.determinant(); 

            FE.cache.B += linear_int[i].second * 1.0/6.0 * B.transpose() * B * dJ; 
            FE.cache.C += linear_int[i].second * 1.0/6.0 * N.transpose() * N * dJ;
            
            ++nbr;
        }
    } 

    for (int t; t < heat_triang_int.size(); ++t)
    {
        FE.cache.SF_s[surf] = Shape_Func(heat_triang_int[t].first, heat_triang_int[t].second, -1.0).transpose();
        ++surf;
    }
}