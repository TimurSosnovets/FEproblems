#include "LQube_IP.hpp"
#include "Output.hpp"
// Eigen
#include <Sparse>
#include <SparseQR>
// STL
#include <chrono>

/*Исходные данные*/
// Node n1(Point(0,0,0), 1), n2(Point(400,0,0), 2), n3(Point(400,0.06,0), 3), n4(Point(0,0.06,0), 4); // Низ элемента
// Node n5(Point(0,0,0.06), 5), n6(Point(400,0,0.06), 6), n7(Point(400,0.06,0.06), 7), n8(Point(0,0.06,0.06), 8); // Верх элемента
Node n1(Point(0,0,0), 1), n2(Point(0.01,0,0), 2), n3(Point(0.01,0.01,0), 3), n4(Point(0,0.01,0), 4); // Низ элемента
Node n5(Point(0,0,0.006), 5), n6(Point(0.01,0,0.006), 6), n7(Point(0.01,0.01,0.006), 7), n8(Point(0,0.01,0.006), 8); // Верх элемента
std::vector<Node*> Vertices = {&n1, &n2, &n3, &n4, &n5, &n6, &n7, &n8};
double J = 0.01 * 0.01;

Eigen::Vector<double, 8> test_temps = {300, 400, 500, 600, 300, 400, 500, 600};

/*Тестирование*/
int main()
{
    /*Инициализация*/
    LQube element(Vertices, AMg_6);

    auto GCM = element.Cond_Mat(test_temps); // Матрица теплопроводности
    auto GDM = element.Damp_Mat(test_temps); // Матрица теплоёмкости
    auto T_elem = element.Element_Temp(test_temps); // Репрезентативная температура элемент
    auto F = element.Heat_Load_Surf(1e5, 0, test_temps, J); // Вектор нагрузок

    /*Вывод тестируемых значений*/
    std::cout << "\n\nElement temperature: " << T_elem << "\n\nConductivity matrix:\n" << GCM << "\n\nDamping matrix\n" << GDM << "\n\nLoad 1:\n" << F << std::endl;
    
    /*Решение задачи стационарной теплопроводности*/
    auto Lh = GCM; // Левая часть уравнения
    auto Rh = F; // Правая часть уравнения

    // Закрепление
    for (int i = 4; i < 8; ++i)
    {
        Rh(i) = 300;
        for (int j = 0; j < 8; ++j)
        {
            if (i == j) {Lh(i,j) = 1;}
            else {Lh(i,j) = 0;}
        }
    }

    // Решение
    Eigen::SparseQR<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>> solver;
    Eigen::SparseMatrix<double> A = Lh.sparseView();
    solver.compute(A);
    if (solver.info() != Eigen::Success) 
    {
        std::cout << "Decomposition failed!" << std::endl;
        std::cin.get();  // Чтобы не закрывалось окно консоли при проблеме
    }
    auto Nodal_temps = solver.solve(Rh);

    // Вывод результата
    std::cout << "\n\nSolution:\n" << Nodal_temps;
    std::cin.get();

}