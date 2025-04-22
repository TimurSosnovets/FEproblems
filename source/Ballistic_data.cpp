#include "Ballistic_data.hpp"
#ifndef DATA_DIR
#define DATA_DIR "../Data"  // Fallback for IDE analysis
#endif

// Helper function to locate data file
std::filesystem::path Ballistic_data::find_data_file(const std::string& filename)
{
    // 1. First check build directory (if DATA_DIR_BUILD is defined)
    #ifdef DATA_DIR_BUILD
    {
        std::filesystem::path build_path(DATA_DIR_BUILD);
        build_path /= filename;
        if (std::filesystem::exists(build_path)) {
            return build_path;
        }
    }
    #endif

    // 2. Check source data directory (DATA_DIR is always defined)
    std::filesystem::path source_path(DATA_DIR);
    source_path /= filename;
    if (std::filesystem::exists(source_path)) {
        return source_path;
    }

    // 3. Check current directory as last resort
    if (std::filesystem::exists(filename)) {
        return filename;
    }

    throw std::runtime_error("Cannot find data file '" + filename + 
                           "' in:\n" +
                           #ifdef DATA_DIR_BUILD
                           "- Build directory: " + std::string(DATA_DIR_BUILD) + "\n" +
                           #endif
                           "- Source directory: " + std::string(DATA_DIR) + "\n" +
                           "- Current directory");
}

// Constructor
Ballistic_data::Ballistic_data(const std::string& csv_filename)
{
    std::filesystem::path file_path = find_data_file(csv_filename);
    // std::cout << "Loading ballistic data from: " << file_path << std::endl;
    parse_CSV(file_path);
    
    if (velocities.empty()) {
        throw std::runtime_error("No valid data loaded from file: " + file_path.string());
    }
}

// CSV parsing implementation
void Ballistic_data::parse_CSV(const std::filesystem::path& filepath)
{
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filepath.string());
    }

    std::string line;
    size_t line_num = 0;
    while (std::getline(file, line)) {
        line_num++;
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string value;
        std::vector<double> row_values;

        try {
            while (std::getline(ss, value, ';')) {
                if (!value.empty()) {
                    row_values.push_back(std::stod(value));
                }
            }

            if (row_values.size() >= 3) {
                velocities.push_back(row_values[0]);
                densities.push_back(row_values[1]);
                knudsen.push_back(row_values[2]);
            } else {
                std::cerr << "Warning: Line " << line_num 
                         << " has insufficient data (expected 3 values, got " 
                         << row_values.size() << ")" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error parsing line " << line_num << ": " << e.what() << std::endl;
            continue;  // Skip bad lines but continue processing
        }
    }

    if (velocities.size() != densities.size() || velocities.size() != knudsen.size()) {
        throw std::runtime_error("Data size mismatch in file: " + filepath.string());
    }
}

// Interpolation and getter methods remain the same
double Ballistic_data::interpolate(const std::vector<double>& data, double query_time) const
{
    const double position = (query_time - start_time) / time_step;
    const size_t index = static_cast<size_t>(position);
    const double alpha = position - index;

    if (index >= data.size() - 1) return data.back();
    return data[index] * (1.0 - alpha) + data[index + 1] * alpha;
}

double Ballistic_data::get_Velocity(double time) const { return interpolate(velocities, time); }
double Ballistic_data::get_Density(double time) const { return interpolate(densities, time); }
double Ballistic_data::get_Knudsen(double time) const { return interpolate(knudsen, time); }