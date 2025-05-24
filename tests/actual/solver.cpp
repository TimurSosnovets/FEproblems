#include "TFE_model.hpp"
#include "Output.hpp"
#include "smth.hpp"
#include "Ballistic_data.hpp"

int main()
{
    /*Геометрия*/
    // Layers layer({0.015, 0.045, 0.002}, {5, 5, 5});
    // Geometry geom(5, 5, 5, 5);
    // int c_phi = 6;
    // /*КЭ модель*/
    // TFE_model DM_FE(geom, layer);
    // make_model_advance(DM_FE, layer, geom, c_phi);
    TFE_model DM_FE("elements.ini");
    logger::log("Model has been made successfully!");
    std::cin.get();

    DM_FE.mesh_info();

    DM_FE.mesh_check();

    logger::log("Successful mesh check!");

    auto nodal_temps = DM_FE.transient_analisys();
    DM_FE.create_mesh_file("const_heat.vtu", nodal_temps);
    // Save_xlsx(nodal_temps);
    std::cin.get();
    return 0;
}