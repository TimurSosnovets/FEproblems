#include "TFE_model.hpp"
#include "Output.hpp"
#include "smth.hpp"
#include "Ballistic_data.hpp"

int main()
{
    /*Геометрия*/
    TFE_model DM_FE("solver_config.ini");
    DM_FE.mesh_info();
    DM_FE.mesh_check();

    auto nodal_temps = DM_FE.transient_analisys("solver_config.ini");
    DM_FE.create_mesh_file("const_heat.vtu", nodal_temps);
    
    // Save_xlsx(nodal_temps);
    std::cin.get();
    return 0;
}