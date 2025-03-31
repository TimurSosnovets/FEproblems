#pragma once
// STL
#include <cmath>
#include <iostream>
#include <string>

//Функции сравнения
bool are_close_enough(double a, double b, double tol=pow(10.0,-10.0)) 
{return (abs((a-b))<=tol);};

bool is_close_to_zero(double a, double tol=pow(10.0,-10.0)) 
{return (are_close_enough(a, 0.0f, pow(10.0,-10.0)));};

bool is_close_to_one(double a, double tol=pow(10.0,-10.0)) 
{return (are_close_enough(a, 1.0f, pow(10.0,-10.0)));};

struct Vec2D
{
    double u;
    double v;
    double norm;
    bool is_normal = false;
    double sine;
    double cosine;

    void set_norm() 
    {
        norm = sqrt(pow(u,2.0f) + pow(v,2.0f));
        is_normal = is_close_to_one(norm);
    }

    Vec2D(double U, double V) : u(U), v(V)
    {
        set_norm();
        sine = v / norm;
        cosine = u / norm;
    }

    void normalize() 
    {
        u = u / (norm);
        v = v / (norm);
        norm = 1.0f;
        is_normal = true;
    }
    
    Vec2D perpendicular() const 
    {
        return Vec2D(-v,u);
    }

    void scale_by(double factor) 
    {
        u = u * factor;
        v = v * factor;
        set_norm();
    }

    void set_length(double length) 
    {
        normalize();
        scale_by(length);
    }

    std::pair<double, double> p2p(const std::pair<double, double>& base) const
    {
        double x = base.first + u;
        double y = base.second + v;
        return std::make_pair(x, y);
    }
};

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
