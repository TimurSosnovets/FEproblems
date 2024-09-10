#pragma once
#include "2dVectors.h"
#include "tparam.h"

using namespace std;

class Line {
    private:
        Point base;
        Vector direction;
    public:
        void get_info() {
            cout << "Line: base is ";
            base.get_info();
            cout << " and direction is ";
            direction.get_info();
        }
        
        explicit Line(const Point base, const Vector direction): 
            base(base),
            direction(direction) {
            }

        bool is_parallel_to(const Line& other) const {
            return direction.is_parallel_to(other.direction);
        }
        
        bool is_perpendicular_to(const Line& other) const {
            return direction.is_perpendicular_to(other.direction);
        }

        Line perpendicular_through(const Point footing) const {
            return Line(footing, direction.perpendicular());
        }

        Line parallel_through(const Point footing) const {
            return Line(footing, direction);
        }

        double p_intersection_coord(const Line& other) const {
            Vector d1 = direction;
            Vector d2 = other.direction;

            double cross_prod = d1.cross(d2);
            Vector delta = make_vector_between(base, other.base);

            double t = (delta.u*d2.v - delta.v*d2.u)/cross_prod;

            return t;
        }

        bool is_intersection_with(const Line& other) const {
            Vector d1 = direction;
            Vector d2 = other.direction;

            if (d1.is_parallel_to(d2)) return false;
            else return true;
        }

        Point intersection_with(const Line& other) {
            double t = p_intersection_coord(other);
            return base.displaced(direction, t);
        }
};