#include "smth.hpp"

// Градусы в радианы
double deg2rad(double deg)
{
    return (deg / 180) * M_PI;
}

double rad2deg(double rad)
{
    return (rad / M_PI) * 180;
}

//Функции сравнения
bool are_close_enough(double a, double b, double tol) 
{return (abs((a-b))<=tol);};
bool is_close_to_zero(double a, double tol) 
{return (are_close_enough(a, 0.0f, pow(10.0,-10.0)));};
bool is_close_to_one(double a, double tol) 
{return (are_close_enough(a, 1.0f, pow(10.0,-10.0)));};

/*Вектор в двумерном пространстве*/
void Vec2D::set_norm() 
{
    norm = sqrt(pow(u,2.0f) + pow(v,2.0f));
    is_normal = is_close_to_one(norm);
}

    Vec2D::Vec2D(double U, double V) : u(U), v(V)
{
    set_norm();
    sine = v / norm;
    cosine = u / norm;
}

    void Vec2D::normalize() 
{
    u = u / (norm);
    v = v / (norm);
    norm = 1.0f;
    is_normal = true;
}
    
    Vec2D Vec2D::perpendicular() const 
{
    return Vec2D(-v,u);
}

void Vec2D::scale_by(double factor) 
{
    u = u * factor;
    v = v * factor;
    set_norm();
}

void Vec2D::set_length(double length) 
{
    normalize();
    scale_by(length);
}

std::pair<double, double> Vec2D::p2p(const std::pair<double, double>& base) const
{
    double x = base.first + u;
    double y = base.second + v;
    return std::make_pair(x, y);
}

Vec2D operator+ (const Vec2D& v1, const Vec2D& v2) 
{
    return Vec2D (v1.u+v2.u, v1.v+v2.v);
};
Vec2D operator- (const Vec2D& v1, const Vec2D& v2) 
{
    return Vec2D (v1.u-v2.u, v1.v-v2.v);
};
bool operator== (const Vec2D& v1, const Vec2D& v2) 
{
    return are_close_enough(v1.u,v2.u)&&are_close_enough(v1.v,v2.v);
};


/*Создание сетки*/
// Y-координата в плоскости xOy (Сфера)
double r_circle(double x, double h, double R_sphere)
{
    return sqrt( pow(R_sphere - h, 2) - pow(x - R_sphere, 2) );
}

// Y-координата в плоскости xOy (Прямая)
double r_line(double x, double h, Point base1, Point base2)
{
    double x1 = base1.x, x2 = base2.x, y1 = base1.y, y2 = base2.y;
    // Вектор смещения по нормали
    Vec2D V(x2 - x1, y2 - y1);
    Vec2D normal = V.perpendicular();
    normal.set_length(h);
    // Новые точки (смещенные по нормали)
    std::pair<double, double> new_base1 = normal.p2p({x1, y1}), new_base2 = normal.p2p({x2, y2});
    x1 = new_base1.first;
    x2 = new_base2.first;
    y1 = new_base1.second;
    y2 = new_base2.second;
    return y1 + ((x - x1) / (x2 - x1)) * (y2 - y1);
}

// Модель слоёв
Layers::Layers(std::array<double, 3> t, std::array<int, 3> f) : thickness(t), fragment(f)
{
    THCK = std::accumulate(thickness.begin(), thickness.end(), 0.0);
    FRNT = std::accumulate(fragment.begin(), fragment.end(), 0);
    for (int i = 0; i < 3; ++i) {step[i] = thickness[i] / fragment[i];}
}
    
double Layers::depth(int iter) const
{
    if (iter < fragment[0]) {return iter * step[0];}
    else if (iter < fragment[0] + fragment[1]) {return thickness[0] + (iter - fragment[0]) * step[1];}
    else return thickness[0] + thickness[1] + (iter - fragment[0] - fragment[1]) * step[2];
}

const Material* Layers::get_material(int iter) const
{
    if (iter < fragment[0]) {return &GC_2500;}
    // else if (iter < fragment[0] + fragment[1]) {return &TZMK_10;}
    else if (iter < fragment[0] + fragment[1]) {return &GC_2500;}
    else return &AMg_6;
}

std::string* Layers::get_name(int iter)
{
    if (iter < fragment[0]) {return &names[0];}
    else if (iter < fragment[0] + fragment[1]) {return &names[1];}
    else return &names[2];
}

// Модель геометрии
std::string* Geometry::get_name(int iter)
{
    if (iter < FE_sph) {return &names[0];}
    else if (iter < FE_cone1 + FE_sph) {return &names[1];}
    else if (iter < FE_cone2 + FE_cone1 + FE_sph) {return &names[2];}
    else if (iter < FE_cyl + FE_cone2 + FE_cone1 + FE_sph) {return &names[3];}
    else return &names[4];
}

Geometry::Geometry(const int Sphere, const int Cone1, const int Cone2, const int Cylinder) :
FE_sph(Sphere), FE_cone1(Cone1), FE_cone2(Cone2), FE_cyl(Cylinder), FE_all(Sphere + Cone1 + Cone2 + Cylinder) {}

// Угол между векторами
double compute_angle(Eigen::Vector3d& V1, Eigen::Vector3d& V2)
{
    Eigen::Quaterniond q;
    q.setFromTwoVectors(V1.normalized(), V2.normalized());
    Eigen::AngleAxisd angleAxis(q);
    return angleAxis.angle();
}

Eigen::Vector3d compute_surf_normal(const double x, const double y, const double z, const Geometry& geom)
{
    // Угол в плоскости YoZ
    const double r = std::hypot(y, z);
    const double cos_phi = y / r;
    const double sin_phi = z / r;

    // Угол в плоскости XoY
    double cos_psi, sin_psi;

    if (x == 0)
    {
        return Eigen::Vector3d(-1, 0, 0);
    }
    else if (x < geom.x_refers[0])
    {
        cos_psi = (geom.R_sphere - x) / (geom.R_sphere);
        sin_psi = sqrt(1 - cos_psi*cos_psi);
    }
    else if (x <= geom.x_refers[1])
    {
        cos_psi = cos(deg2rad(69));
        sin_psi = sin(deg2rad(69));
    }
    else if (x <= geom.x_refers[2])
    {
        cos_psi = cos(deg2rad(82));
        sin_psi = sin(deg2rad(82));
    }
    else if (x < geom.x_refers[3])
    {   
        cos_psi = 0;
        sin_psi = 1;
    }
    else
    {
        cos_psi = -1;
        sin_psi = 0;
    }

    return Eigen::Vector3d(-cos_psi, sin_psi * cos_phi, sin_psi * sin_phi);
}

// Угол между нормалью к поверхности и скоростью аппарата
double heat_angle(const double x, const double y, const double z, const Geometry& geom, const double alpha)
{
   Eigen::Vector3d Velocity(-cos(alpha), -sin(alpha), 0);

   Eigen::Vector3d Surface_normal = compute_surf_normal(x, y, z, geom);

   return compute_angle(Velocity, Surface_normal);
}

// Нагрузка
double heat_load(const double vel, const double dens, const double Kn, const double angle, const double Dm)
{
    /*Радиационная составляющая*/
    double q_r = 2.195 * 1e-22 * pow(vel, 7.9) * pow(dens, 1.2) * pow(Dm, 0.49);

    if (angle > M_PI / 2.0) {return q_r;}

    /*Конвективная составляющая*/
    double q_lam = 3.3 * 1e-5 * pow(vel, 3.2) * sqrt(dens / Dm) * (0.1 + 0.9 * pow(cos(angle), 2));
    double q_turb = 1.06 * 1e-4 * pow(vel, 3.19) * pow(pow(dens, 4) / Dm, 0.2) * (15 * pow(sin(angle), 2) - 14 * pow(sin(angle), 4));

    double q_conv_dens;
    if (q_lam >= q_turb)
    { q_conv_dens = q_lam; } else { q_conv_dens = q_turb; }

    if (Kn < 0.01) { return q_conv_dens + q_r; }

    double q_conv_amend = 0.5 * dens * pow(vel, 3.0) * pow(cos(angle), 3.0);
    double q_conv_mixed = (q_conv_dens + Kn * q_conv_amend) / (1 + Kn);

    return q_conv_mixed + q_r;
}
