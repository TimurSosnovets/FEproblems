#include "../../headers/thermal/2d_plate.hpp"
#include "../../lib/eigen-3.4.0/Eigen/Sparse"
#include "../../lib/eigen-3.4.0/Eigen/SparseQR"

int main() 
{
    // Исходные значения
    const double Length_X = 800.0, Length_Y = 8.0;
    const int Quantity_W = 1;
    const std::array<double, 3> Length_Layer = {2.0, 2.0, 2.0};
    const std::array<int, 3> Quantity_L = {2, 2, 2};
    const std::array<Material, 3> Materials = {GC_2500, TZMK_10, AMg_6};

    // Модель пластины 
    LPlate Plate(Materials, Length_X, Length_Layer, Quantity_W, Quantity_L);

    

    return 0;
};