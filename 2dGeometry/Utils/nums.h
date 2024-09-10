#pragma once
#include <cmath>

//Функции сравнения
bool are_close_enough(double a, double b, double tol=pow(10.0,-10.0)) 
{return (abs((a-b))<=tol);};

bool is_close_to_zero(double a, double tol=pow(10.0,-10.0)) 
{return (are_close_enough(a, 0.0f, pow(10.0,-10.0)));};

bool is_close_to_one(double a, double tol=pow(10.0,-10.0)) 
{return (are_close_enough(a, 1.0f, pow(10.0,-10.0)));};