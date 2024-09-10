#pragma once
#include "2dSize.h"
#include "2dPoint.h"

class Rect {
    private: 
        Point origin;
        Size size;
        double left;
        double right;
        double bottom;
        double top;
        double area;
        double perimeter;
    public:
        void get_info() {
            std::cout << "Rectangle: origin is ";
            origin.get_info();
            std::cout << " height X width is " << size.height << "X" << size.width << "." << std::endl;
            std::cout << "Area is " << area << ", perimeter is " << perimeter << "." << std::endl;
        }

        Point get_origin() {
            return origin;
        }

        Rect(const Point rgn, const Size sz) : origin(rgn), size(sz) {
            left = origin.x;
            right = origin.x + size.width;
            bottom = origin.y;
            top = origin.y + size.height;
            area = size.width * size.height;
            perimeter = 2 * (size.width + size.height);
        }

};