#include "../../headers/thermal/Plate_2d.hpp"
#include "../../lib/eigen-3.4.0/Eigen/Sparse"
#include "../../lib/eigen-3.4.0/Eigen/SparseQR"

double q = 100000.0;

int main() 
{
    // Исходные значения
    const double Length_X = 800, Length_Y = 8;
    const int Quantity_X = 1, Quantity_Y = 1;

    // Тестирование
    std::cout << " AMg-6 lambda: " << AMg_6.get_TCC(300);

    // Модель пластины
    Plate_2d Plate(AMg_6, Length_X, Length_Y, Quantity_X, Quantity_Y);
    // std::cout << "\n\n Global conductivity matrix:\n" << Plate.GCM();
    // std::cout << "\n\n Local conductivity matrix:\n" << Plate.FEs()[0].value().first.Cond_Mat();

    return 0;
}
