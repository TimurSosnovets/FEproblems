#include "TFE_model.hpp"
#include "Output.hpp"
#include "smth.hpp"
#include "Ballistic_data.hpp"

int main()
{
    /*Геометрия*/
    Layers layer({0.015, 0.045, 0.002}, {1, 1, 1});
    Geometry geom(10, 5, 5, 5);
    int c_phi = 4;
    // Проверка
    std::cout << "Geometry check:\n" << "X: " << geom.x_refers[0] << " " <<  geom.x_refers[1] << " " << geom.x_refers[2] << " " << geom.x_refers[3] << ";\n";
    std::cout << "R: " << geom.r_refers[0] << " " <<  geom.r_refers[1] << " " << geom.r_refers[2] << " " << geom.r_refers[3] << ";\n";
    
    /*КЭ модель*/
    TFE_model DM_FE(10, 10, 10);
    fill_FEmodel(DM_FE, layer, geom, c_phi);
    logger::log("Model has been made successfully!");
    std::cin.get();

    // Проверка нагрузки
    logger::log("Heat load (W/m^2):");
    auto next_x = [layer, c_phi] (int node) -> int 
    {
        if (node <= layer.FRNT + 1) {return  node + layer.FRNT + 1;}
        else {return node + (layer.FRNT + 1) * c_phi;}
    };

    double x, y, z, angle;
    double vel, dens, Kn;
    double time = 1;
    // std::cout << "Current path: " << std::filesystem::current_path() << std::endl;
    Ballistic_data data("Ballistics_CD.csv");
    vel = data.get_Velocity(time);
    logger::log("Velocity " + std::to_string(vel));
    dens = data.get_Density(time);
    logger::log("Density " + std::to_string(dens));
    Kn = data.get_Knudsen(time);
    logger::log("Knudsen " + std::to_string(Kn));
    int nbr = 1;
    double at_ang = deg2rad(20);

    Eigen::Vector3d v1 (-1, 0, 0), v2(-1, 0, 0);
    logger::log("Test angle " + std::to_string(compute_angle(v1, v2)));
    logger::log("");
    logger::log("===Sphere===");
    for (int i = 0; i <= geom.FE_sph; ++i)
    {   
        logger::log("Node " + std::to_string(DM_FE.Nodes()[nbr - 1].global_number()));
        x = DM_FE.Nodes()[nbr - 1].coords().x;
        y = DM_FE.Nodes()[nbr - 1].coords().y;
        z = DM_FE.Nodes()[nbr - 1].coords().z;
        angle = heat_angle(x, y, z, geom, at_ang);
        logger::log("Heat angle " + std::to_string(rad2deg(angle)));
        // angle = 0;

        logger::log(std::to_string(heat_load(vel, dens, Kn, angle)));
        logger::log("");
        nbr = next_x(nbr);
    }
    logger::log("============");

    logger::log("===First cone===");
    for (int i = 0; i < geom.FE_cone1; ++i)
    {
        logger::log("Node " + std::to_string(DM_FE.Nodes()[nbr - 1].global_number()));
        x = DM_FE.Nodes()[nbr - 1].coords().x;
        y = DM_FE.Nodes()[nbr - 1].coords().y;
        z = DM_FE.Nodes()[nbr - 1].coords().z;
        angle = heat_angle(x, y, z, geom, at_ang);
        logger::log("Heat angle " + std::to_string(rad2deg(angle)));

        logger::log(std::to_string(heat_load(vel, dens, Kn, angle)));
        logger::log("");
        nbr = next_x(nbr);
    }
    logger::log("================");

    logger::log("===Second cone===");
    for (int i = 0; i < geom.FE_cone2; ++i)
    {
        logger::log("Node " + std::to_string(DM_FE.Nodes()[nbr - 1].global_number()));
        x = DM_FE.Nodes()[nbr - 1].coords().x;
        y = DM_FE.Nodes()[nbr - 1].coords().y;
        z = DM_FE.Nodes()[nbr - 1].coords().z;
        angle = heat_angle(x, y, z, geom, at_ang);
        logger::log("Heat angle " + std::to_string(rad2deg(angle)));

        logger::log(std::to_string(heat_load(vel, dens, Kn, angle)));
        logger::log("");
        nbr = next_x(nbr);
    }
    logger::log("=================");

    logger::log("===Cylinder===");
    for (int i = 1; i < geom.FE_cyl; ++i)
    {
        logger::log("Node " + std::to_string(DM_FE.Nodes()[nbr - 1].global_number()));
        x = DM_FE.Nodes()[nbr - 1].coords().x;
        y = DM_FE.Nodes()[nbr - 1].coords().y;
        z = DM_FE.Nodes()[nbr - 1].coords().z;
        angle = heat_angle(x, y, z, geom, at_ang);
        logger::log("Heat angle " + std::to_string(rad2deg(angle)));

        logger::log(std::to_string(heat_load(vel, dens, Kn, angle)));
        logger::log("");
        nbr = next_x(nbr);
    }
    logger::log("==============");

    logger::log("===Bottom===");
    logger::log("Node " + std::to_string(DM_FE.Nodes()[nbr - 1].global_number()));
    x = DM_FE.Nodes()[nbr - 1].coords().x;
    y = DM_FE.Nodes()[nbr - 1].coords().y;
    z = DM_FE.Nodes()[nbr - 1].coords().z;
    angle = heat_angle(x, y, z, geom, at_ang);
    logger::log("Heat angle " + std::to_string(rad2deg(angle)));

    logger::log(std::to_string(heat_load(vel, dens, Kn, angle)));
    nbr = next_x(nbr);
    logger::log("============");

    logger::log("Whole temps");
    // std::cout << temps;
    std::cin.get();
    return 0;

}