#include "TFE_model.hpp"
#include "Output.hpp"
#include "smth.hpp"
#include "Ballistic_data.hpp"

int main() 
{
    Layers layer({0.015, 0.045, 0.002}, {10, 10, 1});
    Geometry geom(23, 12, 15, 10);
    int c_phi = 36;
    /*КЭ модель*/
    TFE_model DM_FE(geom, layer);
    make_model_advance(DM_FE, layer, geom, c_phi);
    logger::log("Model has been made successfully!");
    std::cin.get();

    // DM_FE.mesh_info();

    DM_FE.mesh_check();

    logger::log("Successful mesh check!");

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
        Eigen::VectorXd step_load = DM_FE.get_surface_load(time);
        // Eigen::VectorXd step_load = Eigen::VectorXd::Ones(surf_elem);
        elemental_load.emplace_back(time, step_load);
        time += time_step;
    }
    logger::log("Elemental load vectors are populated!");
    std::cin.get();
    DM_FE.create_surface_mesh_file("surface", elemental_load);
    return 0;
}