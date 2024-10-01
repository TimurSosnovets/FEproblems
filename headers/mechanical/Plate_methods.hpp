#pragma once
#include "Triangle_2dof.hpp"
#include "Structs.hpp"
#include <iostream>


class Plate_triangulated;
// Разбиение на узлы
inline void Nodes_creation(std::vector<std::pair<Point, int>>& Nodes, const int DoF, const double a, const double b, const int m, const int n) {
    Nodes.reserve(DoF);            
        int k = 1;
        for (int i = 0; i < m+1; ++i) {
            for (int j = 0; j < n+1; ++j) {
                Nodes.emplace_back(std::make_pair(Point(a/m * i, b/n * j), k));                    
                k++;
            }
        }
}

// Ассамблирование
template <typename T>
typename std::enable_if<std::is_same<T, TriangleFE>::value, void>::type
Assembly (Eigen::MatrixXd& GSM, const int& DoF, const std::vector<std::pair<T, int>>& FEs) {
    GSM = Eigen::MatrixXd::Zero(DoF, DoF);

    std::vector<int> Gnn; // Вектор индексов коэффициентов в глобальной матрице
    if constexpr (std::is_same<T, TriangleFE>::value) {
        Gnn.reserve(6);
    }
    // if constexpr (std::is_same<T, QuadFE>::value) {
    //     gnn.reserve(8);
    // }

    for (const auto& FE: FEs) {
        // Заполняем вектор (учитывается, что номера начинались с единицы)
        for (const auto& node : FE.first.verts()) {
            Gnn.emplace_back(2 * (node.second - 1));
            Gnn.emplace_back(2 * (node.second - 1) + 1);
        }

        // Заполнение глобальной матрицы
        double* K_ij = FE.first.K().data(); // Указатель на локальную матрицу жесткости элемента
        for (const auto& colm : Gnn) {
            for (const auto& row : Gnn) {
                GSM (row, colm) += *K_ij;
                ++K_ij;
            }
        }

        Gnn.clear(); // Очищаем вектор
    }

    Gnn.shrink_to_fit();
};

template <typename T>
typename std::enable_if<std::is_same<T, Plate_triangulated>::value, Solution>::type
solve(T& Plate, const LBC LBC) {
    // Заполнение вектора нагрузок          
    Eigen::VectorXd F = Eigen::VectorXd::Zero(Plate.GSM().rows()); // Вектор узловых нагрузок
    for (auto& nd_force : LBC.Forces) {
        const int& nbr1 = std::get<0>(nd_force);
        F[2 * (nbr1 - 1)] = std::get<1>(nd_force);
        F[2 * (nbr1 - 1) + 1] = std::get<2>(nd_force);
    }

    // Вектор узловых сил от температурных деформаций
    double NTemp = 293; // Температура при НУ
    Eigen::VectorXd Temp = Eigen::VectorXd::Zero(Plate.FEs().size()); // Вектор относительных температур элементов
    // Вытаскиваем узловые температуры            
    int n = 3;
    for (const auto& FE : Plate.FEs()) {                                   
        for (const auto& Ndl_T : LBC.Nodal_Temp) {
            for (const auto& vert : FE.first.verts()) {
                if (vert.second == Ndl_T.first) {
                    Temp[FE.second - 1] += (Ndl_T.second - NTemp) / n;
                    break;
                }
            }
        }
    }
    // Вытаскиваем температуры элементов
    for (const auto& Elt_T : LBC.Element_Temp) {
        if (Temp[Elt_T.first - 1] < (Elt_T.second - NTemp)) {
            Temp[Elt_T.first - 1] = Elt_T.second - NTemp;
        }
    }
//std::cout << "Elements temperature vector:\n" << Temp << std::endl << std::endl;
    // Переводим температуру элемента в узловые силы
    Eigen::Vector3d Temp_Strain;
    Eigen::VectorXd Temp_Forces(6);
    for (int i = 0; i < Plate.FEs().size(); ++i) {
        double a = Plate.FEs()[i].first.Mat().CLTE(); // КЛТР
        Temp_Strain << a * Temp[i], a * Temp[i], 0;
        Temp_Forces = (Plate.thickness()) * (Plate.FEs()[i].first.Square()) * (Plate.FEs()[i].first.B().transpose() * Plate.FEs()[i].first.Mat().D() * Temp_Strain);
        for (int j = 0; j < 3; ++j) {
            F[2 * (Plate.FEs()[i].first.verts()[j].second - 1)] += Temp_Forces[2 * j];
            F[2 * (Plate.FEs()[i].first.verts()[j].second - 1) + 1] += Temp_Forces[2 * j + 1];
        }
    }
//std::cout << "Forces vector:\n" << F << "\n\n";
    // Закрепление узлов в матрице жесткости
    auto K = Plate.GSM();
    for (auto& nd_dof : LBC.DOF) {
        const int& nbr2 = std::get<0>(nd_dof);
        if (std::get<1>(nd_dof)) {
            K.row(2 * (nbr2 - 1)) = Eigen::RowVectorXd::Zero(Plate.GSM().rows());
            K.col(2 * (nbr2 - 1)) = Eigen::VectorXd::Zero(Plate.GSM().rows());
            K(2 * (nbr2 - 1), 2 * (nbr2 - 1)) = 1;
            F[2 * (nbr2 - 1)] = 0;
        }
        if (std::get<2>(nd_dof)) {
            K.row(2 * (nbr2 - 1) + 1) = Eigen::RowVectorXd::Zero(Plate.GSM().rows());
            K.col(2 * (nbr2 - 1) + 1) = Eigen::VectorXd::Zero(Plate.GSM().rows());
            K(2 * (nbr2 - 1) + 1, 2 * (nbr2 - 1) + 1) = 1;
            F[2 * (nbr2 - 1) +1] = 0;
        }
    }
//std::cout << "Right hand vector:\n" << F;
    //auto nodal_displacements = K.colPivHouseholderQr().solve(F);
    Eigen::VectorXd nodal_displacements = K.partialPivLu().solve(F); // Глобальный ВУОП

    // Массивы векторов напряжений и деформаций
    std::vector<std::pair<Eigen::Vector3d, int>> strains; // Вектор деформаций (в любой точке) каждого КЭ
    std::vector<std::pair<Stress2d, int>> stresses; // Вектор напряжений (в любой точке) каждого КЭ
    strains.reserve(Plate.FEs().size());
    stresses.reserve(Plate.FEs().size());

    int NDoF = 6;
    // if constexpr (std::is_same<T, Plate_triangulated>::value) {
    //     NDoF = 6;
    // }      
    int el = 0; // Номер конечного элемента (нужен для вектора температур)
    for (const auto& FE : Plate.FEs()) {
        // Получение перемещений вершин КЭ       
        Eigen::VectorXd vertices_displacements(NDoF); // Узловые перемещения КЭ (ЛВУОП)
        double* v_d = vertices_displacements.data(); // Указатель на первый элемент ЛВУОП
        for (const auto& node : FE.first.verts()) {
            int* nbr3 = new int(node.second);                
            *v_d = nodal_displacements[2 * (*nbr3 - 1)];
            ++v_d;
            *v_d = nodal_displacements[2 * (*nbr3 - 1) +1];
            ++v_d;
            delete nbr3;
        }

        // Вычисление векторов полной деформации и напряжения (в центральной точке) каждого КЭ
        Eigen::Vector3d el_strain = FE.first.B() * vertices_displacements;
        Eigen::Vector3d el_temp_strain;
        el_temp_strain << FE.first.Mat().CLTE() * Temp[el], FE.first.Mat().CLTE() * Temp[el], 0;  
        Eigen::Vector3d el_stress = FE.first.Mat().D() * (el_strain - el_temp_strain);
        //Eigen::Vector3d el_stress = FE.first.Mat().D() * (el_strain);
        ++el;
        std::cout << "Number of elements: " << el << std::endl;
        strains.emplace_back(std::make_pair(el_strain, FE.second));
        stresses.emplace_back(std::make_pair(Stress2d(el_stress), FE.second));
    }

return Solution(nodal_displacements, strains, stresses);
};