#include "TFE_model.hpp"
#include "Output.hpp"
#include "smth.hpp"
#include "Ballistic_data.hpp"

int main()
{
    /*Геометрия*/
    Layers layer({0.015, 0.045, 0.002}, {1, 1, 1});
    Geometry geom(1, 1, 1, 1);
    int c_phi = 4;
    /*КЭ модель*/
    TFE_model DM_FE(geom, layer);
    make_model(DM_FE, layer, geom, c_phi);
    logger::log("Model has been made successfully!");
    std::cin.get();

    DM_FE.mesh_info();

    DM_FE.mesh_check();
    logger::log("Successful mesh check!");

    DM_FE.transient_analisys();
    std::cin.get();
    return 0;
}