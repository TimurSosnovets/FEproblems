#include "TFE_model.hpp"
#include "Output.hpp"
#include "smth.hpp"
#include "Ballistic_data.hpp"

int main()
{
    /*Геометрия*/
    Layers layer({0.015, 0.045, 0.002}, {10, 10, 1});
    Geometry geom(23, 12, 15, 10);
    int c_phi = 36;
    /*КЭ модель*/
    TFE_model DM_FE(geom, layer);
    make_model_advance(DM_FE, layer, geom, c_phi);
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