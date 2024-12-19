#pragma once
#include <vector>
#include <algorithm>
#include <stdexcept>

// Линейная интерполяция (два вектора)
double interp_lin(const std::vector<double>& temperatures, const std::vector<double>& values, double T);

// Линейная интерполяция (один вектор пар)
double interp_lin(const std::vector<std::pair<double, double>>& data, double T);


class Material
{
    private:
        const std::vector<std::pair<double, double>> TCC; // Коэффициент теплопроводности
        const std::vector<std::pair<double, double>> SHC; // Удельная теплоёмкость
        const double rho; // Плотность 
        const bool const_prop; // Маркер зависимости от температуры
    public:
        // Конструктор для непостоянных параметров
        Material(std::vector<std::pair<double, double>> tcc, std::vector<std::pair<double, double>> shc, double r);;
        // Конструктор для постоянных параметров
        Material(double tcc, double shc, double r);

        // Коэффициент теплопроводности при заданной температуре
        double get_TCC(double T) const;

        // Удельная теплоёмкость при заданной температуре
        double get_SHC(double T) const;

        // Плотность
        double dens() const;

};


extern const Material GC_2500;
extern const Material TZMK_10;
extern const Material AMg_6;