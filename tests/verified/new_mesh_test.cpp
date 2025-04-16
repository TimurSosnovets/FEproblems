#include "TFE_model.hpp"
#include "Output.hpp"
#include "smth.hpp"

int main()
{
    /*Геометрия*/
    Layers layer({0.015, 0.045, 0.002}, {1, 1, 1});
    Geometry geom(10, 10, 10, 20);
    int c_phi = 12;
    /*КЭ модель*/
    TFE_model DM_FE(geom, layer);
    make_model_advance(DM_FE, layer, geom, c_phi);
    logger::log("Model has been made successfully!");
    std::cin.get();

    DM_FE.mesh_info();

    DM_FE.mesh_check();
    logger::log("Successful mesh check!");

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