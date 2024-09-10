#pragma once
#include <cmath>
#include <iostream>
#include "Utils/nums.h"
using namespace std;


//Класс "Вектор"
class Vector {
    protected:
        double u;
        double v;
        double norm;
        bool is_normal;
        double sine;
        double cosine;
    public:
        friend class Point;
        friend class Segment;
        friend class Line;
        friend bool are_close_enough (double a, double b, double tol);
        friend bool is_close_to_one (double a, double tol);
        friend bool is_close_to_zero (double a, double tol);
        friend Vector operator+ (const Vector& v1, const Vector& v2);
        friend Vector operator- (const Vector& v1, const Vector& v2);
        friend bool operator== (const Vector& v1, const Vector& v2); 
        friend Vector scaled_by (const Vector& v, const double factor);

        void get_info() {
            cout << "Vector: " << "{" << u << ", " << v <<"} | Norm=" << norm << " | Is normal?: ";
            if (is_normal) cout << "Yes." << endl; else cout << "No." << endl;
        }

        void set_norm() {
            norm=sqrt(pow(u,2.0f)+pow(v,2.0f));
            is_normal=is_close_to_one(norm);
        }

        explicit Vector(double u, double v) : u(u), v(v) {
            set_norm();
            sine=this->v/norm;
            cosine=this->u/norm;
        }

        void normalize() {
            double* n = new double(norm);
            u=u/(*n);
            v=v/(*n);
            delete n;
            norm=1.0f;
            is_normal=true;
        }

        void scale_by(double factor) {
            u=u*factor;
            v=v*factor;
            set_norm();
        }

        Vector normalized() const {
            Vector V=Vector(u,v);
            V.normalize();
            return V;
        }

        Vector scaled_by(double factor) const {
            Vector V=Vector(u,v);
            V.scale_by(factor);
            return V;
        }
        
        Vector with_length(double length) const {
            Vector V=Vector(u,v);
            V.normalize();
            V.scale_by(length);
            return V;
        }

        double dot(const Vector V) const {
            return (u*V.u+v*V.v);
        }

        double projection_over(const Vector direction) const {
            return dot(direction.normalized());
        }

        double cross(const Vector V) const {
            return (u*V.v-v*V.u);
        }
        
        bool is_parallel_to(const Vector V) const {
            return is_close_to_zero(cross(V));
        }

        bool is_perpendicular_to(const Vector V) const {
            return is_close_to_zero(dot(V));
        }

        double angle_value_to(const Vector V) const {
            double dot_product=dot(V);
            double norm_product=norm*V.norm;
            return acos(dot_product/norm_product);
        }
        
        double angle_to(const Vector V) const {
            double value=angle_value_to(V);
            double sgn=cross(V)/abs(cross(V));
            return sgn*value;
        }

        Vector rotated_radians(double radians) const {
            double c=cos(radians);
            double s=sin(radians);
            return Vector(u*c-v*s,u*s+v*c);
        }

        Vector perpendicular() const {
            return Vector(-v,u);
        }

        Vector opposite() const {
            return Vector(-u,-v);
        }



};

Vector operator+ (const Vector& v1, const Vector& v2) {
    return Vector (v1.u+v2.u, v1.v+v2.v);
};
Vector operator- (const Vector& v1, const Vector& v2) {
    return Vector (v1.u-v2.u, v1.v-v2.v);
};
bool operator== (const Vector& v1, const Vector& v2) {
    return are_close_enough(v1.u,v2.u)&&are_close_enough(v1.v,v2.v);
};
Vector scaled_by (const Vector& v, double factor) {
    return Vector (v.u*factor, v.v*factor);
};

