#pragma once
#include "2dVectors.h"
#include "2dLine.h"
#include "tparam.h"


class Segment {
    private:
        Point start;
        Point end;
        Vector direction_vector;
        Vector direction_versor;
        Vector normal_versor;
        Point middle;
        double length;
        Line bisector;
    public:

        // Для тестов
        double get_length() {
            return length;
        }

        Point& get_middle() {
            return middle;
        }

        Line& get_bisector() {
            return bisector;
        }

        friend bool operator== (const Segment& s1, const Segment& s2); 
        
        void get_info() {
            cout << "Segment: start is ";
            start.get_info();
            cout << ", end is ";
            end.get_info();  
            cout << ". The length is " << length << "." << endl;
        }

        Point point_at(double t) {
            ensure_valid(t);
            return start.displaced(direction_vector, t);
        }

        explicit Segment(const Point start, const Point end):  
            start(start), 
            end(end), 
            direction_vector(make_vector_between(start, end)), 
            direction_versor(make_versor_between(start, end)), 
            normal_versor(direction_versor.perpendicular()), 
            middle(point_at(MIDDLE)), 
            length(start.distance_to(end)),
            bisector(Line(middle, normal_versor)) {}

        Point closest_point_to(const Point& p) const {
            Vector v = make_vector_between(start, p);
            Vector d = direction_versor;
            double vs = v.projection_over(d);

            if (vs < 0) return start;
            
            if (vs > length) return end;
            
            return start.displaced(d, vs);
        }

        double distance_to(const Point& p) const {
            Point closest = closest_point_to(p);
            return p.distance_to(closest);
        }

        pair<double, double> p_intersection_coord(const Segment& other) const {
            Vector d1 = direction_vector;
            Vector d2 = other.direction_vector;

            double cross_prod = d1.cross(d2);
            Vector delta = other.start - start;

            double t1 = (delta.u*d2.v - delta.v*d2.u)/cross_prod;
            double t2 = (delta.u*d1.v - delta.v*d1.u)/cross_prod;

            return {t1, t2};
        }

        bool is_intersection_with(const Segment& other) const {
            Vector d1 = direction_vector;
            Vector d2 = other.direction_vector;

            if (d1.is_parallel_to(d2)) return false;

            pair<double, double> tpair = p_intersection_coord(other);
            if (is_valid(tpair.first) && is_valid(tpair.second)) return true;
            else return false;
        }

        Point intersection_with(const Segment& other) {
            double t = p_intersection_coord(other).first;
            return point_at(t);
        }

};

bool operator== (const Segment& s1, const Segment& s2) {
    if (s1.start == s2.start && s1.end == s2.end) return true;
    else return false;
};