#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
// STL
#include <iostream>
#include <string>
#include <numeric>
// Eigen
#include <Dense>
#include <Geometry>
// Current project
#include "FE_entities.hpp"
#include "Output.hpp"

/*Специальные функции для удобства*/
// Градусы в радианы
double deg2rad(double deg);
double rad2deg(double rad);

//Функции сравнения
bool are_close_enough(double a, double b, double tol=pow(10.0,-10.0));
bool is_close_to_zero(double a, double tol=pow(10.0,-10.0));
bool is_close_to_one(double a, double tol=pow(10.0,-10.0));

/*Вектор в двумерном пространстве*/
struct Vec2D
{
    double u;
    double v;
    double norm;
    bool is_normal = false;
    double sine;
    double cosine;

    void set_norm();
    void normalize();
    Vec2D perpendicular() const;
    Vec2D rotate_ccw_rad(double angle_rad) const;
    Vec2D opposite() const;
    void scale_by(double factor);
    void set_length(double length);
    std::pair<double, double> p2p(const std::pair<double, double>& base) const;
    Point move_by(Point& p) const;

    Vec2D(double U, double V);
    Vec2D(Point& start, Point& end);
};

Vec2D operator+ (const Vec2D& v1, const Vec2D& v2);
Vec2D operator- (const Vec2D& v1, const Vec2D& v2);
bool operator== (const Vec2D& v1, const Vec2D& v2);

/*Геометрия модели аппарата*/
// Y-координата в плоскости xOy (Сфера)
double r_circle(double x, double h, double R_sphere);

// Y-координата в плоскости xOy (Прямая)
double r_line(double x, double h, Point base1, Point base2);

// Модель слоёв
struct Layers
{
    std::array<double, 3> thickness;
    std::array<int, 3> fragment;
    std::array<double, 3> step;
    std::array<std::string, 3> names = {"GC-2500", "TZMK-10", "AMg6"};
    double THCK;
    int FRNT;
    bool force_AMg;

    Layers(std::array<double, 3> t, std::array<int, 3> f, bool is_AMg = false);
    double depth(int iter) const;
    const Material* get_material(int iter) const;
    std::string* get_name(int iter);
};

struct nodes_map
{
    int break_point = -1;
    int sound_point = -1;
    std::vector<int> under_break_point;
    std::vector<int> under_sound_point;
    std::vector<int> keel;
};

// Модель геометрии
struct Geometry
{
    // Геометрия
    double R_sphere = 0.336;
    std::array<double, 4> x_refers = {R_sphere * (1 - cos(deg2rad(69))), (10000.0 - 4800.0 - 2320.0) / 1000.0, (10000.0 - 2320.0) / 1000.0, (10000.0) / 1000.0};
    std::array<double, 4> r_refers = {R_sphere * sin(deg2rad(69)), R_sphere * sin(deg2rad(69)) + (x_refers[1] - x_refers[0]) * tan(deg2rad(21)), 2.0 - 2.4 * tan(deg2rad(8)), 4.0 / 2.0};
    std::array<std::string, 5> names = {"Sphere", "First cone", "Second cone", "Cylinder", "Bottom"};

    // КЭ разбиение
    int FE_sph, FE_cone1, FE_cone2, FE_cyl, FE_all;
    double step_sph, step_cone1, step_cone2, step_cyl;
    nodes_map map;
    std::string* get_name(int iter);
    Geometry(const int Sphere, const int Cone1, const int Cone2, const int Cylinder);
};

// Нормаль к поверхности аппарата
Eigen::Vector3d compute_surf_normal(const double x, const double y, const double z, const Geometry& geom);

// Угол между векторами
double compute_angle(Eigen::Vector3d& V1, Eigen::Vector3d& V2);

// Угол между нормалью к поверхности и скоростью аппарата
double heat_angle(const double x, const double y, const double z, const Geometry& geom, const double alpha = deg2rad(20));

// Нагрузка
double heat_load(const double vel, const double dens, const double Kn, const double angle, const double Dm = 4.0);

// Характерные узлы
struct output_temps
{   
    const double bal_ang = deg2rad(20); 
    int brake_point;
    std::vector<int> keel;
    std::vector<int> under_brake;
};