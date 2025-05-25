#include "TFE_model.hpp"
#include "Output.hpp"
#include "smth.hpp"
#include "Ballistic_data.hpp"

int main()
{
    // /*Создание дириктории с результатами*/
    // if (!filesystem_utils::create_directory()) {
    //     std::cerr << "Failed to create Results directory. Continuing without logging to file." << std::endl;
    // }

    /*Создание модели*/
    TFE_model DM_FE("solver_config.ini");
    DM_FE.mesh_info("Results/mesh_log.txt");
    DM_FE.mesh_check();

    /*Решение*/
    auto nodal_temps = DM_FE.transient_analisys("solver_config.ini", "Results/time_log.txt");

    /*Запись решения*/
    DM_FE.create_mesh_file("Results", "", nodal_temps);
    // Save_xlsx(nodal_temps);

    /*Финиш*/
    logger::log("Press enter to close window...");
    std::cin.get();
    return 0;
}