#pragma once
#include "2dPoint.h"
#include "2dVector.h"
using namespace std;

Vector make_vector_between (const Point& p1, const Point& p2) {
    return (p2-p1);
};

Vector make_versor (const double u, const double v) {
    return Vector(u,v).normalized();
};

Vector make_versor_between (const Point& p1, const Point& p2) {
    return make_vector_between(p1,p2).normalized();
};