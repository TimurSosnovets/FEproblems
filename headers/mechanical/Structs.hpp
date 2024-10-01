#pragma once
#include <vector>
#include "2dStress(PSS).hpp"

// Структура граничных условий
struct LBC {
std::vector<std::tuple<int, bool, bool>> DOF;
std::vector<std::tuple<int, double, double>> Forces;
std::vector<std::pair<int, double>> Nodal_Temp;
std::vector<std::pair<int, double>> Element_Temp;

LBC() {};
};

// Структура решения задачи
struct Solution {
    Eigen::VectorXd Nd_Dspl; //Узловые перемещения
    std::vector<std::pair<Eigen::Vector3d, int>> FE_Strains; // Вектор деформаций для (центральной точки) каждого КЭ 
    std::vector<std::pair<Stress2d, int>> FE_Stresses; // Вектор напряжений для (центральной точки) каждого КЭ

    Solution(const Eigen::VectorXd Dspl, const std::vector<std::pair<Eigen::Vector3d, int>> Strains, const std::vector<std::pair<Stress2d, int>> Stresses)
    : Nd_Dspl(Dspl), FE_Strains(Strains), FE_Stresses(Stresses) {} 
};