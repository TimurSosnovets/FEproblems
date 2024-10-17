#pragma once
#include "LQuad_IP_FE.hpp"

template <typename U, int N>
void Assembly(Eigen::MatrixXd& GCM, const int& DOF, std::array<std::pair<LQuad, int>, N>* FEs)
{
    GCM = Eigen::MatrixXd::Zero(DOF, DOF); // Создание нулевой матрицы нужного размера
 
    int row, colm; // Индексы коэффициента в ГМТ
    int i = 0, j = 0; // Индексы коэффициентов в ЛМТ
    
    for (const auto& FE : *FEs) // Обход по элементам
    {
        const Eigen::Matrix<double, 4, 4>& Local_H = FE.first.Cond_Mat();
        for (const auto& rowlp : FE.first.Vertices()) 
        {
            row = rowlp.get().second - 1;
            for (const auto& colmlp : FE.first.Vertices())
            {
                colm = colmlp.get().second - 1;
                GCM(row, colm) += Local_H(i, j); // Непосредственный перенос значения из ЛМТ в ГМТ на нужную позицию
                ++j;
            }
            ++i;
            j = 0;
        }
        i = 0;
    }
};

