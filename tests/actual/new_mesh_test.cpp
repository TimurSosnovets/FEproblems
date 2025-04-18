#include "TFE_model.hpp"
#include "Output.hpp"
#include "smth.hpp"

int main()
{
    /*Геометрия*/
    Layers layer({0.015, 0.045, 0.002}, {10, 10, 1}, true);
    Geometry geom(23, 15, 10, 5);
    int c_phi = 12;
    /*КЭ модель*/
    TFE_model DM_FE(geom, layer);
    make_model_advance(DM_FE, layer, geom, c_phi);
    logger::log("Model has been made successfully!");
    std::cin.get();

    DM_FE.mesh_info();
    DM_FE.export_to_vtk("mesh.vtu", false);

    std::cin.get();
    DM_FE.mesh_check();
    logger::log("Successful mesh check!");

    std::cout << "Break point: " << geom.map.break_point << std::endl;
    std::cout << "Sound point: " << geom.map.sound_point << std::endl;
    
    /*Закрепление*/
    double constraint_temp = 300;
    std::vector<std::pair<int, double>> LBC;
    for (const auto& node : DM_FE.Nodes())
    {
        if (node.global_number() % (layer.FRNT + 1) == 0) {LBC.emplace_back(node.global_number() - 1, constraint_temp);}
    }

    /*Решение стационарной задачи*/
    Eigen::VectorXd temps = DM_FE.steady_state_analysis(LBC, 1e5, true);
    logger::log("Whole temps");
    std::cout << temps;
    std::cin.get();
    return 0;
}