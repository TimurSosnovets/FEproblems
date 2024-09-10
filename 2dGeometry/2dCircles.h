#pragma once
#include "2dCircle.h"


Circle make_circle_from_points(const Point a, const Point b, const Point c) {
    Line& chord_one_bisec = Segment(a, b).get_bisector();
    Line& chord_two_bisec = Segment(b, c).get_bisector();
    Point center = chord_one_bisec.intersection_with(chord_two_bisec);
    double radius = center.distance_to(a);
    return Circle(center, radius);
};