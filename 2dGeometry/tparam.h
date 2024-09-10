#pragma once
#include <cmath>
#include <iostream>

using namespace std;

double MIN = 0.0;
double MIDDLE = 0.5;
double MAX = 1.0;

double make(double value) {
    if (value < MIN) return MIN;
    if (value > MAX) return MAX;
    return value;
};

bool is_valid(double t) {
    if (t < MIN || t > MAX) return false;
    else return true;
};

void ensure_valid(double t) {
    if (!is_valid(t)) throw std::invalid_argument("Expected t to be in [0, 1]");  
};
