#include "../../headers/thermal/Descent_module_2d.hpp"
#include "../../lib/eigen-3.4.0/Eigen/Sparse"
#include "../../lib/eigen-3.4.0/Eigen/SparseQR"

double R_cr, R_c1, R_c2, X_cr, X_c1, X_c2, X_cl;
int N_x, N_r, N_cr, N_cn1, N_cn2, N_cl, N_gc, N_tzmk;
double q = 100000;


int main() 
{
    // Геометрия аппарата
    R_cr = 336; R_c2 = 2000; R_c1 = 2000 - 4800 * tan(8 / 180.0 * M_PI); 
    X_cr = 336 * (1 - cos(69 / 180.0 * M_PI)); X_c1 = 10000 - 2320 - 4800; X_c2 = 10000 - 2320; X_cl = 10000;
    DM_Geom2d Nes_corp_geom = DM_Geom2d(R_cr, R_c1, R_c2, X_cr, X_c1, X_c2, X_cl);
    
    // Сетка
    N_cr = 10; N_cn1 = 7; N_cn2 = 5; N_cl = 3;
    N_gc = 3; N_tzmk = 5;
    N_x = N_cr + N_cn1 + N_cn2 + N_cl; N_r = N_gc + N_tzmk + 1;
    std::cout << "Estimate length: " << (N_x) * (N_r) << std::endl;
    DM_FEmodel Nes_crop_fe = DM_FEmodel(Nes_corp_geom, 15, 45, 3, N_cr, N_cn1, N_cn2, N_cl, N_gc, N_tzmk);

    // Нагрузка
    std::cout << std::endl << "Load\n";
    Eigen::VectorXd F = Eigen::VectorXd::Zero(Nes_crop_fe.GCM().cols()); // Глобальный вектор тепловых узловых нагрузок
    Eigen::Vector<double, 4> F_local; // Локальный вектор узловых нагрузок
    for (int i = 0; i < (N_x * N_r); i += N_r) //Элементы с 0 начинались
    {   
        if (!Nes_crop_fe.FEs()[i].has_value()) {std::cout << "\nFE: " << i; return 0;}
        auto& p1 = Nes_crop_fe.FEs()[i].value().first.Vertices()[0].get();
        auto& p2 = Nes_crop_fe.FEs()[i].value().first.Vertices()[1].get();
        F(p1.second - 1) += q * Point::distance_to(p1.first, p2.first) / 2000.0;
        F(p2.second - 1) += q * Point::distance_to(p1.first, p2.first) / 2000.0;
        std::cout << "El " << i+1 << ", verts: " << p1.second << " , " << p2.second<< std::endl;
    };
    // for (int i = 0; i < (N_x * N_r); i += N_r) //Элементы с 0 начинались
    // {
    //     F_local = Nes_crop_fe.FEs()[i].value().first.Heat_Load(q/1000);
    //     std::cout << "El " << i+1 << " F = [" << F_local(0) << " " << F_local(1) << " " << F_local(2) << " " << F_local(3) << "]" << std::endl;
    //     for (int nbr = 0; nbr < 4; ++nbr)
    //     {
    //         F(Nes_crop_fe.FEs()[i].value().first.Vertices()[nbr].get().second - 1) = F_local(nbr);
    //     }
    // }
    

    // Закрепление
    std::cout << std::endl << "\nBC\n";
    Eigen::MatrixXd gcm = Nes_crop_fe.GCM();
    int sz = gcm.cols();
    for (int i = N_r-1; i < (N_x * N_r); i += N_r)
    {
        auto& p1 = Nes_crop_fe.FEs()[i].value().first.Vertices()[2].get();
        auto& p2 = Nes_crop_fe.FEs()[i].value().first.Vertices()[3].get();
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
};