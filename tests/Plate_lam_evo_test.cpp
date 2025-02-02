#include "../include/2d_plate.hpp"
#include "../lib/Eigen/Sparse"
#include "../lib/Eigen/SparseQR"
#include <chrono>
#include <OpenXLSX.hpp>

// Function to save Eigen vector to Excel
void SaveToExcel(const Eigen::VectorXd& data, const std::string& filename) 
{
    using namespace OpenXLSX;

    try {
        XLDocument doc;
        doc.create(filename, XLForceOverwrite);
        auto wks = doc.workbook().worksheet("Sheet1");

        for (int i = 0; i < data.size(); ++i) {
            wks.cell(i + 1, 1).value() = data(i);
        }

        doc.save();
        doc.close();
        std::cout << "Data successfully saved to " << filename << std::endl;
    } 
    catch (const std::exception& e) {
        std::cerr << "Error while saving to Excel: " << e.what() << std::endl;
    }
}


    // Исходные значения
        // Геометрия (м)
    const double Length_X = 0.35;
    const std::array<double, 3> Length_Layer = {0.015, 0.045, 0.002};
        // Физические свойства
    const std::array<Material, 3> Materials = {GC_2500, TZMK_10, AMg_6};
    const double eps = 0.9;
    // const double q = 2.076e6; // Тепловой поток (Вт/м^2)
    const double q = 3.987e5;
        // Разбиение
    const int Quantity_W = 1;
    const std::array<int, 3> Quantity_L = {3, 9, 2};
    int dof = (std::accumulate(Quantity_L.begin(), Quantity_L.end(), 0) + 1) * (Quantity_W + 1);
    
    // Модель пластины 
    LPlate Plate(Materials, Length_X, Length_Layer, Quantity_W, Quantity_L);

    // Эволюционный расчёт
    Eigen::VectorXd Evo_calc(LPlate Plate, const double q, const double eps, const double T_init, const double t, const double dt)
    {   
        // Начальные условия
        Eigen::VectorXd Nodal_temps = T_init * Eigen::VectorXd::Ones(dof); // Вектор узловых значений на i-м шаге

        for (int i = 0; i < t / dt; ++i)
        {
            Eigen::MatrixXd Lh = (Plate.GCM(Nodal_temps) + (2 / dt) * Plate.GDM(Nodal_temps)); // Матрица левой части уравнения
            Eigen::VectorXd Rh = ((2 / dt) * Plate.GDM(Nodal_temps) - Plate.GCM(Nodal_temps)) * Nodal_temps + 2 * Plate.F(q, 0.9, Nodal_temps); // Правая часть уравнения

            // Решение на i-м шаге
            Eigen::SparseQR<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>> solver;
            Eigen::SparseMatrix<double> A = Lh.sparseView();
            solver.compute(A);
            if (solver.info() != Eigen::Success) 
            {
                std::cout << "Decomposition failed!" << std::endl;
                std::cin.get();  // Чтобы не закрывалось окно консоли при проблеме
                return Eigen::VectorXd::Zero(dof); 
            }
            Nodal_temps = solver.solve(Rh);
        }     

        return Nodal_temps;
    }


int main() 
{
    // Старт таймера
    auto start = std::chrono::high_resolution_clock::now();

    auto Nodal_temps = Evo_calc(Plate, q, eps, 300, 325, 1e-1);

    // Остановка таймера
    auto end = std::chrono::high_resolution_clock::now();

    // Вычисление времени в секундах
    std::chrono::duration<double> elapsed_seconds = end - start;

    // Вывод решения и времени расчёта
    std::cout << "\n\n Solution: \n" << Nodal_temps << std::endl;
    std::cout << "\nExecution time: " << elapsed_seconds.count() << " seconds" << std::endl;

    // Проверка на вшивость
    if (Nodal_temps.hasNaN()) 
    {
        std::cerr << "NaN detected in solution!" << std::endl;
        std::cin.get();
        return -1;
    }

    SaveToExcel(Nodal_temps, "NDL_TEMPS.xlsx");
    std::cin.get();
    return 0;
};