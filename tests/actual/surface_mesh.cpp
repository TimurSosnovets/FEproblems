#include "TFE_model.hpp"
#include "Output.hpp"
#include "smth.hpp"
#include "Ballistic_data.hpp"

int main() 
{
    /*Создание модели*/
    TFE_model DM_FE("solver_config.ini");
    DM_FE.mesh_info("Results/mesh_log.txt");
    DM_FE.mesh_check();

    std::vector<std::pair<double, Eigen::VectorXd>> elemental_load;
    double time = 0, time_step = 1.0, max_time = 2140.0;
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