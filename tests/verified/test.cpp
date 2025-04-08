#include "TFE_model.hpp"
#include "Output.hpp"
#include "smth.hpp"
#include "Ballistic_data.hpp"

int main()
{
    /*Геометрия*/
    Layers layer({0.015, 0.045, 0.002}, {1, 1, 1});
    Geometry geom(15, 10, 10, 5);
    int c_phi = 12;
    // Проверка
    std::cout << "Geometry check:\n" << "X: " << geom.x_refers[0] << " " <<  geom.x_refers[1] << " " << geom.x_refers[2] << " " << geom.x_refers[3] << ";\n";
    std::cout << "R: " << geom.r_refers[0] << " " <<  geom.r_refers[1] << " " << geom.r_refers[2] << " " << geom.r_refers[3] << ";\n";
    
    /*КЭ модель*/
    TFE_model DM_FE(geom, layer);
    
    // Заполнение массива узлов
    make_model(DM_FE, layer, geom, c_phi);
    logger::log("Model has been made successfully!");
    std::cin.get();

    DM_FE.mesh_info();
    std::cin.get();

    DM_FE.mesh_check();
    logger::log("Successful mesh check!");

    /*Решение*/
    // Ввод значений
    double constraint_temp = 300;
    std::vector<std::pair<int, double>> LBC;
    for (const auto& node : DM_FE.Nodes())
    {
        if (node.global_number() % (layer.FRNT + 1) == 0) {LBC.emplace_back(node.global_number() - 1, constraint_temp);}
    }

    Eigen::VectorXd temps = DM_FE.steady_state_analysis(LBC, 1e5, true);

    std::vector<int> nodes_keel;
    for (const auto& node : DM_FE.Nodes())
    {
        if ((node.coords().z < 1e-8) && (node.coords().y < 0))
        {
            nodes_keel.emplace_back(node.global_number());
        }
    }

    Eigen::VectorXd out(nodes_keel.size());
    int it = 0;
    for (const auto& nbr : nodes_keel)
    {
        out(it) = temps(nbr - 1);
        ++it;
    }
    
    auto next_x = [layer, c_phi] (int node) -> int 
    {
        if (node <= layer.FRNT + 1) {return  node + layer.FRNT + 1;}
        else {return node + (layer.FRNT + 1) * c_phi;}
    };
    int nbr = 0;
    logger::log("Nodal temperatures (phi = 0, h = 0):");
    
    logger::log("===Sphere===");
    for (int i = 0; i < geom.FE_sph; ++i)
    {
        logger::log(std::to_string(temps[nbr]));
        nbr = next_x(nbr);
    }
    logger::log("============");

    logger::log("===First cone===");
    for (int i = 0; i < geom.FE_cone1; ++i)
    {
        logger::log(std::to_string(temps[nbr]));
        nbr = next_x(nbr);
    }
    logger::log("================");

    logger::log("===Second cone===");
    for (int i = 0; i < geom.FE_cone2; ++i)
    {
        logger::log(std::to_string(temps[nbr]));
        nbr = next_x(nbr);
    }
    logger::log("=================");

    logger::log("===Cylinder===");
    for (int i = 0; i < geom.FE_cyl; ++i)
    {
        logger::log(std::to_string(temps[nbr]));
        nbr = next_x(nbr);
    }
    logger::log("==============");

    logger::log("===Bottom===");
    nbr = DM_FE.Nodes().size() - layer.FRNT - 1;
    logger::log(std::to_string(temps[nbr]));
    logger::log("============");

    logger::log("Whole temps");
    // std::cout << temps;
    std::cin.get();
    return 0;

}