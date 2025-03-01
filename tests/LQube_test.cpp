#include "LQube_IP.hpp"
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
std::vector<Node*> Vertices = {&n1, &n2, &n3, &n4, &n5, &n6, &n7, &n8};
double surface_area = 0.01 * 0.01;

Eigen::Vector<double, 8> test_temps = {300, 400, 500, 600, 300, 400, 500, 600};

/*Тестирование*/
int main()
{
    try {
        /*Инициализация*/
        TFE_model model(1, 1, 1);
        logger::log("Model created succesfully!");
        std::cin.get();

        for (const auto& node : Vertices) {model.add_node(*node);}
        logger::log("Nodes were added succesfully!");
        std::cin.get();

        model.add_element(ElementType::LQube, Vertices, 1, &AMg_6, true, surface_area);
        logger::log("The element was added succesfully!");
        std::cin.get();

        model.pre_calculate();
        logger::log("Mesh calculated succesfully!");
        std::cin.get();

        model.mesh_check();
        logger::log("Mesh has been checked succesfully!");
        std::cin.get();

        model.surface_check();
        logger::log("Surface has been checked succesfully!");
        std::cin.get();

        model.mesh_info();
        std::cin.get();
    
        auto GCM = model.GCM(test_temps); // Матрица теплопроводности
        logger::log("GCM has been computed succesfully!");
        std::cin.get();

        auto GDM = model.GDM(test_temps); // Матрица теплоёмкости
        logger::log("GDM has been computed succesfully!");
        std::cin.get();

        auto F = model.NLV(10e5, 0, test_temps); // Вектор нагрузок
        logger::log("NLV has been computed succesfully!");
        std::cin.get();

        /*Вывод тестируемых значений*/
        std::cout << "\n\nConductivity matrix:\n" << GCM.toDense() << "\n\nDamping matrix\n" << GDM.toDense() << "\n\nLoad 1:\n" << F.toDense() << std::endl;
        std::cin.get();
        
        /*Решение задачи стационарной теплопроводности*/
        auto Lh = GCM; // Левая часть уравнения
        auto Rh = F; // Правая часть уравнения

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
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.get(); // Wait for user input

    return 0;
}