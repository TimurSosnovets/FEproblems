#include "TFE_model.hpp"
#include "Output.hpp"
#include "smth.hpp"
#include "Ballistic_data.hpp"
#include "INIReader.h"
#include "Parser_helpers.hpp"

int main() 
{
    /*Инициализация ввода данных*/
    INIReader reader("solver_config.ini");
    if (reader.ParseError() < 0) {
        throw std::runtime_error("Can't load config file: solver_config.ini");
    }
    double max_time         = reader.GetReal("Calculation", "max_time", 10.0);
    float time_step         = reader.GetReal("Calculation", "time_step", 0.01);

    /*Создание модели*/
    TFE_model DM_FE("solver_config.ini");
    DM_FE.mesh_info("Results/mesh_log.txt");
    DM_FE.mesh_check();

    std::vector<std::pair<double, Eigen::VectorXd>> elemental_load;
    double time = 0;
    int step_count = static_cast<int>(max_time / time_step);
    elemental_load.reserve(step_count);
    int surf_elem = 0;
    for (const auto& element : DM_FE.Elements())
    {
        if (element.surface_check()) {++surf_elem;}
    }
    while (time < max_time)
    {
        Eigen::VectorXd step_load = DM_FE.get_surface_load("solver_config.ini", time);
        // Eigen::VectorXd step_load = Eigen::VectorXd::Ones(surf_elem);
        elemental_load.emplace_back(time, step_load);
        time += time_step;
    }
    logger::log("Elemental load vectors are populated!");
    DM_FE.create_surface_mesh_file("Results", "surface", elemental_load);
    return 0;
}