#pragma once
#include <cmath>
#include <iostream>


//Функции сравнения
bool are_close_enough(double a, double b, double tol=pow(10.0,-10.0));

bool is_close_to_zero(double a, double tol=pow(10.0,-10.0));

bool is_close_to_one(double a, double tol=pow(10.0,-10.0));

//Класс "Точка"
class Point {
    private:
        double x;
        double y;
    public:
        explicit Point(double x, double y);
        friend bool are_close_enough (double a, double b, double tol);
        friend bool operator== (const Point& p1, const Point& p2);   
        double distance_to (const Point& p) const;
        double coord_x() const;
        double coord_y() const;
        void get_info();
};

bool operator== (const Point& p1, const Point& p2);
       

