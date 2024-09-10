#pragma once
#include <cmath>
#include "2dPolygon.h"

class Circle {
    private:
        Point center;
        double radius;
        double area;
        double circumference;
    public:

    friend bool operator== (const Circle& c1, const Circle& c2);
    
    void get_info() {
        std::cout << "Circle: center is ";
        center.get_info();
        std::cout << "  radius - " << radius << ", circumference - " << circumference << ", area - " << area << "." << std::endl;
    }

    explicit Circle(Point ctr, double rds) : center(ctr), radius(rds) {
        area = M_PI * pow(radius, 2.0f);
        circumference = 2 * M_PI * radius;
    }

    bool contains_point(const Point position)  {
        return (position.distance_to(center) < radius);
    }

    Point __point_at_angle(const double angle) const {
        return Point(center.x + radius * cos(angle), center.y + radius * sin(angle));
    }

    Polygon to_polygon(const int divisions) const {
        double angle_delta = 2 * M_PI / divisions;
        std::vector<Point> pol_verts;
        pol_verts.reserve(divisions);
        for (int i = 0; i < divisions; ++i) {
            pol_verts.emplace_back(__point_at_angle(angle_delta * i));
        }
        return Polygon(pol_verts);
    }
};

bool operator== (const Circle& c1, const Circle& c2) {
    return ((are_close_enough(c1.radius, c2.radius)) && (c1.center == c2.center));
};