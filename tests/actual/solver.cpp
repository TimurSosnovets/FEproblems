#include "TFE_model.hpp"
#include "Output.hpp"
#include "smth.hpp"
#include "Ballistic_data.hpp"

int main()
{
    /*Геометрия*/
    Layers layer({0.020, 0.040, 0.002}, {8, 16, 2});
    Geometry geom(69, 40, 20, 20);
    int c_phi = 12;
    /*КЭ модель*/
    TFE_model DM_FE(geom, layer);
    make_model_advance(DM_FE, layer, geom, c_phi);
    logger::log("Model has been made successfully!");
    std::cin.get();

    DM_FE.mesh_info();

    DM_FE.mesh_check();
    logger::log("Successful mesh check!");

    DM_FE.transient_analisys();
    std::cin.get();
    return 0;
}