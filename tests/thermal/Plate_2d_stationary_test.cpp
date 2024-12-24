#include "../../headers/thermal/Plate_2d.hpp"
#include "../../lib/eigen-3.4.0/Eigen/Sparse"
#include "../../lib/eigen-3.4.0/Eigen/SparseQR"

double q = 10000.0;

int main() 
{
    // Исходные значения
    const double Length_X = 10.0, Length_Y = 6.0;
    const int Quantity_X = 10, Quantity_Y = 3, DOF = (Quantity_X + 1) * (Quantity_Y + 1);

    // Тестирование
    std::cout << " AMg-6 lambda: " << AMg_6.get_TCC(300);

    // Модель пластины
    Plate_2d Plate(AMg_6, Length_X, Length_Y, Quantity_X, Quantity_Y);
    
    // Формирование вектора нагрузок 
    std::cout << std::endl << "Load\n";
    Eigen::VectorXd F = Eigen::VectorXd::Zero(Plate.GCM().cols()); // Глобальный вектор тепловых узловых нагрузок
    Eigen::Vector<double, 4> F_local; // Локальный вектор узловых нагрузок
    for (int i = 0; i < (Quantity_X * Quantity_Y); i += Quantity_Y) //Элементы с 0 начинались
    {   
        if (!Plate.FEs()[i].has_value()) {std::cout << "\nFE: " << i; return 0;}
        auto& p1 = Plate.FEs()[i].value().first.Vertices()[0].get();
        auto& p2 = Plate.FEs()[i].value().first.Vertices()[1].get();
        F(p1.second - 1) += q * Point::distance_to(p1.first, p2.first) / 2;
        F(p2.second - 1) += q * Point::distance_to(p1.first, p2.first) / 2;
        std::cout << "El " << i+1 << ", verts: " << p1.second << " , " << p2.second<< std::endl;
    };

    // Закрепление
    std::cout << std::endl << "\nBC\n";
    Eigen::MatrixXd gcm = Plate.GCM();
    int sz = gcm.cols();
    for (int i = Quantity_Y-1; i < (Quantity_X * Quantity_Y); i += Quantity_Y) 
    {
        auto& p1 = Plate.FEs()[i].value().first.Vertices()[2].get();
        auto& p2 = Plate.FEs()[i].value().first.Vertices()[3].get();
        gcm.row(p1.second - 1) = Eigen::RowVectorXd::Zero(sz);
        gcm(p1.second - 1, p1.second - 1) = 1;
        gcm.row(p2.second - 1) = Eigen::RowVectorXd::Zero(sz);
        gcm(p2.second - 1, p2.second - 1) = 1;
        std::cout << "El " << i + 1 << ", verts: " << p1.second << " , " << p2.second << std::endl;
        F(p1.second - 1) = 300;
        F(p2.second - 1) = 300;
    }

    // Решение
    std::cout << "\n Right hand vector:\n" << F;
    Eigen::SparseQR<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>> solver;
        Eigen::SparseMatrix<double> GCM_sparse = gcm.sparseView();
        solver.compute(GCM_sparse);
        if (solver.info() != Eigen::Success) 
        {
            std::cout << "Decomposition failed!" << std::endl;
            return -1; // Handle error
        }
        Eigen::VectorXd T = solver.solve(F);

        std::cout << "\n\n Solution: \n" << T << std::endl;

    return 0;
}
