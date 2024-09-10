#pragma once;
#include "Utils/nums.h"

class Size {
    private:
        double width;
        double height;
    public:
        friend class Rect;
        friend bool operator== (const Size& sz1, const Size& sz2);
        Size(const double wdth, const double hght) : width(wdth), height(hght) {}
};

bool operator== (const Size& sz1, const Size& sz2) {
    return ((are_close_enough(sz1.width, sz2.width)) && (are_close_enough(sz1.height, sz2.height)));
};