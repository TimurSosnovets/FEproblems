#include "Ballistic_data.hpp"

// Конструктор
explicit Ballistic_data::Ballistic_data(const std::string& csv_file) 
{
    parse_CSV(csv_file);
    if (velocities.empty()) 
    {
        throw std::runtime_error("No data loaded");
    }
}

// Парсинг файла
void Ballistic_data::parse_CSV(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) 
    {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    std::string line;
    // Skip header if exists
    std::getline(file, line);

    while (std::getline(file, line)) 
    {
        std::stringstream ss(line);
        std::string value;
        double t, v, d, k;

        try 
        {
            std::getline(ss, value, ';');
            t = std::stod(value);
            
            std::getline(ss, value, ';');
            v = std::stod(value);
            
            std::getline(ss, value, ';');
            d = std::stod(value);
            
            std::getline(ss, value);
            k = std::stod(value);

        } catch (...) 
        {
            throw std::runtime_error("Error parsing line: " + line);
        }

        velocities.push_back(v);
        densities.push_back(d);
        knudsen.push_back(k);
    }
}

// Линейная интерполяция с постоянным шагом
double Ballistic_data::interpolate(const std::vector<double>& data, double query_time) const
{
    const double position = (query_time - start_time) / time_step;
    const size_t index = static_cast<size_t>(position);
    const double alpha = position - index;

    if (index >= data.size() - 1) return data.back();
    return data[index] * (1.0 - alpha) + data[index + 1] * alpha;
};

// Значения
double Ballistic_data::get_Velocity(double time) const { return interpolate(velocities, time); }
double Ballistic_data::get_Density(double time) const { return interpolate(densities, time); }
double Ballistic_data::get_Knudsen(double time) const { return interpolate(knudsen, time); }


    