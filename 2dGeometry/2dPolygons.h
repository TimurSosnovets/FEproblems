#pragma once;
#include "2dPolygon.h"

Polygon make_polygon_from_coords(const std::vector<double>& coords) {
    if (coords.size() % 2 != 0) {
        throw std::invalid_argument("Need an even number of coordinates");
    };

    std::vector<Point> vert_arr;
    vert_arr.reserve(coords.size() / 2);
    for (int i = 0; coords.size(); i += 2) {
        vert_arr.emplace_back(Point(coords[i], coords[i+1]));
    }
return Polygon(vert_arr);
};