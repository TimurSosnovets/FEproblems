#include "../../headers/thermal/LQuad_IP_FE.hpp"
#include "../../headers/thermal/TFEA_methods.hpp"
#include "../../lib/eigen-3.4.0/Eigen/Sparse"
#include "../../lib/eigen-3.4.0/Eigen/SparseQR"
    // Узлы
    std::pair<Point, int> n1 {Point(-400, -4), 1};
    std::pair<Point, int> n2 {Point(-400, 0), 2};
    std::pair<Point, int> n3 {Point(-400, 4), 3};
    std::pair<Point, int> n4 {Point(0, -4), 4};
    std::pair<Point, int> n5 {Point(0, 0), 5};
    std::pair<Point, int> n6 {Point(0, 4), 6};
    std::pair<Point, int> n7 {Point(400, -4), 7};
    std::pair<Point, int> n8 {Point(400, 0), 8};
    std::pair<Point, int> n9 {Point(400, 4), 9};
    std::array<std::pair<Point, int>, 9> Nodes = {n1, n2, n3, n4, n5, n6, n7, n8, n9}; // Массив узлов

    // Конечные элементы
    double lambda = 50.8;
    std::array<std::reference_wrapper<std::pair<Point, int>>, 4> Verts1 = {std::ref(n1), std::ref(n4), std::ref(n5), std::ref(n2)};
    LQuad FE1 = LQuad (Verts1, lambda, lambda);
    std::array<std::reference_wrapper<std::pair<Point, int>>, 4> Verts2 = {std::ref(n2), std::ref(n5), std::ref(n6), std::ref(n3)};
    LQuad FE2 = LQuad (Verts2, lambda, lambda);
    std::array<std::reference_wrapper<std::pair<Point, int>>, 4> Verts3 = {std::ref(n4), std::ref(n7), std::ref(n8), std::ref(n5)};
    LQuad FE3 = LQuad (Verts3, lambda, lambda);
    std::array<std::reference_wrapper<std::pair<Point, int>>, 4> Verts4 = {std::ref(n5), std::ref(n8), std::ref(n9), std::ref(n6)};
    LQuad FE4 = LQuad (Verts4, lambda, lambda);

    std::array<std::pair<LQuad, int>, 4> FEs = {
    std::make_pair(FE1, 1),
    std::make_pair(FE2, 2),
    std::make_pair(FE3, 3),
    std::make_pair(FE4, 4)
};
    int main()
    {        
        const int DOF = Nodes.size();
        Eigen::MatrixXd GCM (DOF, DOF);
        int i = 1;
        for (const auto& FE : FEs)
        {
            std::cout << "FE" << i << ". LCM: \n" << FE.first.Cond_Mat() << std::endl;
            ++i;
        }
        Assembly<LQuad, 4>(GCM, DOF, &FEs);
        std::cout << "\n\n GCM: \n" << GCM << std::endl;
        double condition_number = GCM.fullPivLu().rcond();  // Reciprocal of condition number
        std::cout << "Condition number: " << 1.0 / condition_number << std::endl;
        if (GCM.fullPivLu().isInvertible()) {
            std::cout << "Matrix is invertible" << std::endl;
        } else {
            std::cout << "Matrix is singular or nearly singular" << std::endl;
        }

        // Закрепление - ГУ первого рода  
        GCM.row(0) = Eigen::RowVectorXd::Zero(DOF);
        GCM(0,0) = 1;
        GCM.row(3) = Eigen::RowVectorXd::Zero(DOF);
        GCM(3,3) = 1;
        GCM.row(6) = Eigen::RowVectorXd::Zero(DOF);
        GCM(6,6) = 1;

        std::cout << "\n\n GCM with BC: \n" << GCM << "\n Rank: " << GCM.colPivHouseholderQr().rank() << std::endl;
        condition_number = GCM.fullPivLu().rcond();  // Reciprocal of condition number
        std::cout << "Condition number: " << 1.0 / condition_number << std::endl;
        if (GCM.fullPivLu().isInvertible()) {
            std::cout << "Matrix is invertible" << std::endl;
        } else {
            std::cout << "Matrix is singular or nearly singular" << std::endl;
        }
        
        // Вектор правой части
        Eigen::Vector<double, DOF> F;
        F << 300, 0, 2e5, 300, 0, 4e5, 300, 0, 2e5; 
        std::cout << "\n\n Right hand vector: \n" << F << std::endl;

        // Решение
        //GCM.diagonal().array() += 1e-6;
        //auto T = GCM.colPivHouseholderQr().solve(F);
        //auto T = GCM.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(F);
        //auto T = GCM.partialPivLu().solve(F);
        
        Eigen::SparseQR<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>> solver;
        Eigen::SparseMatrix<double> GCM_sparse = GCM.sparseView();
        solver.compute(GCM_sparse);  // A is the sparse matrix
        if (solver.info() != Eigen::Success) {
        std::cout << "Decomposition failed!" << std::endl;
        return -1; // Handle error
        }
        Eigen::VectorXd T = solver.solve(F);

        std::cout << "\n\n Solution: \n" << T << std::endl;


        
    }