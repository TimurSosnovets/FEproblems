#include <json.hpp>
#include <fstream>
#include <vector>
#include <iostream>
#include "TFE_model.hpp"
#include "utils.hpp"

int main() {
    std::string config_file = "config.json";
    std::ifstream config_stream(config_file);
    if (!config_stream.is_open()) {
        throw std::runtime_error("Failed to open config file: " + config_file);
    }
    vtknlohmann::json config;  // Use vtknlohmann instead of nlohmann
    config_stream >> config;
    config_stream.close();

    const auto& mesh_config = config["mesh"];
    std::vector<double> layer_thicknesses = mesh_config["layer_thicknesses"].get<std::vector<double>>();
    std::vector<int> layer_divisions = mesh_config["layer_divisions"].get<std::vector<int>>();
    const auto& geom_config = mesh_config["geometry"];
    int sphere_fe = geom_config["sphere_fe"].get<int>();
    int cone1_fe = geom_config["cone1_fe"].get<int>();
    int cone2_fe = geom_config["cone2_fe"].get<int>();
    int cylinder_fe = geom_config["cylinder_fe"].get<int>();
    int c_phi = mesh_config["c_phi"].get<int>();

    if (sphere_fe <= 0 || cone1_fe <= 0 || cone2_fe <= 0 || cylinder_fe <= 0 || c_phi <= 0) {
        throw std::runtime_error("Invalid geometry parameters in config file");
    }
    if (layer_thicknesses.size() != layer_divisions.size()) {
        throw std::runtime_error("Layer thicknesses and divisions arrays must have the same size");
    }

    Layers layer(layer_thicknesses, layer_divisions);
    Geometry geom(sphere_fe, cone1_fe, cone2_fe, cylinder_fe);

    TFE_model DM_FE(geom, layer);
    make_model_advance(DM_FE, layer, geom, c_phi);
    logger::log("Model has been made successfully!");

    DM_FE.mesh_info();
    DM_FE.mesh_check();
    logger::log("Successful mesh check!");

    auto nodal_temps = DM_FE.transient_analisys(config_file);
    DM_FE.create_mesh_file("mesh", nodal_temps);
    Save_xlsx(nodal_temps);

    std::cin.get();
    return 0;
}