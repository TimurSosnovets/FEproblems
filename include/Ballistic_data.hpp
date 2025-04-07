#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cmath>
#include <iostream>
#include <filesystem>

class Ballistic_data
{
    private:
        const double start_time = 0.0;
        const double time_step = 1.0;
        std::vector<double> velocities; // Скорость СА
        std::vector<double> densities; // Плотность атмосферы
        std::vector<double> knudsen; // Критерий Кнудсена

        void parse_CSV(const std::string& filename);
        double interpolate(const std::vector<double>& data, double query_time) const;

    public:
        // Конструктор из файла .csv
        explicit Ballistic_data(const std::string& csv_file);

        // Значения
        double get_Velocity(double time) const; 
        double get_Density(double time) const;
        double get_Knudsen(double time) const;
};