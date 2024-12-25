#include "../../headers/thermal/2d_plate.hpp"
#include "../../lib/eigen-3.4.0/Eigen/Sparse"
#include "../../lib/eigen-3.4.0/Eigen/SparseQR"

int main() 
{
    // Исходные значения
    const double Length_X = 800.0, Length_Y = 8.0;
    const int Quantity_W = 1;
    const std::array<double, 3> Length_Layer = {2, 2, 2};
    const std::array<int, 3> Quantity_L = {1, 1, 1};
    const std::array<Material, 3> Materials = {GC_2500, TZMK_10, AMg_6};
    int dof = (std::accumulate(Quantity_L.begin(), Quantity_L.end(), 0) + 1) * (Quantity_W + 1);
    const double q = 1e5;
    // Модель пластины 
    LPlate Plate(Materials, Length_X, Length_Layer, Quantity_W, Quantity_L);
    Eigen::VectorXd Nodal_temps = Eigen::VectorXd::Zero(dof);
    for (int i = 0; i < dof; ++i) {Nodal_temps[i] += 300;}
    std::cout << "\nGCM\n" << Plate.GCM(Nodal_temps);
    std::cout << "\n\nGDM\n" << Plate.GDM(Nodal_temps);
    std::cout << "\n\nF\n" << Plate.F(q, 0, Nodal_temps);
 

    return 0;
};