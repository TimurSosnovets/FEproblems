#include "../../headers/thermal/Descent_module_2d.hpp"

double R_cr, R_c1, R_c2, X_cr, X_c1, X_c2, X_cl;


int main() 
{
    R_cr = 336; R_c2 = 2000; R_c1 = 2000 - 4800 * tan(8 / 180.0 * M_PI); 
    X_cr = 336 * (1 - cos(69 / 180.0 * M_PI)); X_c1 = 10000 - 2320 - 4800; X_c2 = 10000 - 2320; X_cl = 10000;
    DM_Geom2d Nes_corp_geom = DM_Geom2d(R_cr, R_c1, R_c2, X_cr, X_c1, X_c2, X_cl);

    std::cout << "\n DOF: " << (5 + 3 + 3 + 2 + 1) * (4 + 4 + 2);
    DM_FEmodel Nes_crop_fe = DM_FEmodel(Nes_corp_geom, 15, 45, 3, 5, 3, 3, 2, 4, 4);
    return 0;
};