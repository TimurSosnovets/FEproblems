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
    std::vector<double> velocities;
    std::vector<double> densities;
    std::vector<double> knudsen;

    void parse_CSV(const std::filesystem::path& filepath);
    double interpolate(const std::vector<double>& data, double query_time) const;

    // Helper function to find data file
    static std::filesystem::path find_data_file(const std::string& filename);

public:
    // Constructor now takes just the filename
    explicit Ballistic_data(const std::string& csv_filename);

    double get_Velocity(double time) const; 
    double get_Density(double time) const;
    double get_Knudsen(double time) const;
};