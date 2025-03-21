#include "LQube_IP.hpp"
#include "LWedge_IP.hpp"
#include "Output.hpp"
#include "TFE_model.hpp"
// Eigen
#include <Sparse>
#include <SparseQR>
// STL
#include <chrono>

/*Исходные данные*/
Node n1(Point(0,0,0), 1), n2(Point(0.01,0,0), 2), n3(Point(0.01,0.01,0), 3), n4(Point(0,0.01,0), 4); // Низ элемента
Node n5(Point(0,0,0.006), 5), n6(Point(0.01,0,0.006), 6), n7(Point(0.01,0.01,0.006), 7), n8(Point(0,0.01,0.006), 8); // Верх элемента

Node w1(Point(0,0,0), 1), w2(Point(0.01,0,0), 2), w3(Point(0,0.01,0), 3); // Низ элемента
Node w4(Point(0,0,0.006), 4), w5(Point(0.01,0,0.006), 5), w6(Point(0,0.01,0.006), 6); // Верх элемента

std::vector<const Node*> Q_Verts = {&n1, &n2, &n3, &n4, &n5, &n6, &n7, &n8};
std::vector<const Node*> W_Verts = {&w1, &w2, &w3, &w4, &w5, &w6};
double Q_surf_area = 0.01 * 0.01;
double W_surf_area = 1/2.0 * 0.01 * 0.01;

Eigen::Vector<double, 8> Q_Temps = {300, 400, 500, 600, 300, 400, 500, 600};
Eigen::Vector<double, 6> W_Temps = {300, 400, 500, 300, 400, 500};

/*Тестирование*/
int main()
{
    try {
        /*Инициализация*/
        TFE_model Q_Model(1, 1, 1);
        TFE_model W_Model(1, 1, 1);
        logger::log("Models are created succesfully!");
        std::cin.get();

        for (const auto& node : Q_Verts) {Q_Model.add_node(*node);}
        for (const auto& node : W_Verts) {W_Model.add_node(*node);}
        logger::log("Nodes were added succesfully!");
        std::cin.get();

        Q_Model.add_element(ElementType::LQube, Q_Verts, 1, &AMg_6, true, Q_surf_area);
        W_Model.add_element(ElementType::LWedge, W_Verts, 1, &AMg_6, true, W_surf_area);
        logger::log("The element was added succesfully!");
        std::cin.get();

        Q_Model.pre_calculate();
        W_Model.pre_calculate();
        logger::log("Mesh calculated succesfully!");
        std::cin.get();

        Q_Model.mesh_check();
        W_Model.mesh_check();
        logger::log("Mesh has been checked succesfully!");
        std::cin.get();

        Q_Model.surface_check();
        W_Model.surface_check();
        logger::log("Surface has been checked succesfully!");
        std::cin.get();

        logger::log("Linear Qube:");
        Q_Model.mesh_info();
        logger::log("Linear Wedge:");
        W_Model.mesh_info();
        std::cin.get();
    
        auto Q_GCM = Q_Model.GCM(Q_Temps); // Матрица теплопроводности
        auto W_GCM = W_Model.GCM(W_Temps);
        logger::log("Q_GCM has been computed succesfully!");
        std::cin.get();

        auto Q_GDM = Q_Model.GDM(Q_Temps); // Матрица теплоёмкости
        auto W_GDM = W_Model.GDM(W_Temps);
        logger::log("Q_GDM has been computed succesfully!");
        std::cin.get();

        auto Q_F = Q_Model.NLV(10e5, 0, Q_Temps); // Вектор нагрузок
        auto W_F = W_Model.NLV(10e5, 0, W_Temps);
        logger::log("NLV has been computed succesfully!");
        std::cin.get();

        /*Вывод тестируемых значений*/
        logger::log("=====Linear Qube=====");
        std::cout << "\n\nConductivity matrix:\n" << Q_GCM.toDense() << "\n\nDamping matrix\n" << Q_GDM.toDense() << "\n\nLoad 1:\n" << Q_F.toDense() << std::endl;
        std::cin.get();
        logger::log("=====Linear Wedge=====");
        std::cout << "\n\nConductivity matrix:\n" << W_GCM.toDense() << "\n\nDamping matrix\n" << W_GDM.toDense() << "\n\nLoad 1:\n" << W_F.toDense() << std::endl;
        std::cin.get();

        /*Решение задачи стационарной теплопроводности (для куба)*/
        logger::log("=====Linear Qube solving=====");
        auto Lh = Q_GCM; // Левая часть уравнения
        auto Rh = Q_F; // Правая часть уравнения

        // Закрепление
        for (int i = 4; i < 8; ++i)
        {
            Rh.coeffRef(i) = 300;
            for (int j = 0; j < 8; ++j)
            {
                if (i == j) {Lh.coeffRef(i, j) = 1;}
                else {Lh.coeffRef(i, j) = 0;}
            }
        }

        std::cout << "\n\nLeft hand of the equation:\n" << Lh.toDense() << "\n\nThe right one:\n" << Rh.toDense() << std::endl;
        std::cin.get();

        // Решение
        Eigen::SparseQR<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>> solver;
        solver.compute(Lh);
        if (solver.info() != Eigen::Success) 
        {
            std::cout << "Decomposition failed!" << std::endl;
            std::cin.get();  // Чтобы не закрывалось окно консоли при проблеме
            return -1; // Exit if decomposition fails
        }
        Eigen::VectorXd Nodal_temps = solver.solve(Rh); // Convert to dense vector

        // Вывод результата
        std::cout << "\n\nSolution:\n" << Nodal_temps << std::endl;

        /*Решение задачи стационарной теплопроводности (для клина)*/
        logger::log("=====Linear Wedge solving=====");
        Lh = W_GCM; // Левая часть уравнения
        Rh = W_F; // Правая часть уравнения

        // Закрепление
        for (int i = 3; i < 6; ++i)
        {
            Rh.coeffRef(i) = 300;
            for (int j = 0; j < 6; ++j)
            {
                if (i == j) {Lh.coeffRef(i, j) = 1;}
                else {Lh.coeffRef(i, j) = 0;}
            }
        }

        std::cout << "\n\nLeft hand of the equation:\n" << Lh.toDense() << "\n\nThe right one:\n" << Rh.toDense() << std::endl;
        std::cin.get();

        // Решение
        solver.compute(Lh);
        if (solver.info() != Eigen::Success) 
        {
            std::cout << "Decomposition failed!" << std::endl;
            std::cin.get();  // Чтобы не закрывалось окно консоли при проблеме
            return -1; // Exit if decomposition fails
        }
        Nodal_temps = solver.solve(Rh); // Convert to dense vector

        // Вывод результата
        std::cout << "\n\nSolution:\n" << Nodal_temps << std::endl;        
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.get(); // Wait for user input

    return 0;
}