#pragma once
#include "FEtypes/Triangle_2dof.h"
#include "Structs.h"

class Plate_triangulated;
// Разбиение на узлы
void Nodes_creation(std::vector<std::pair<Point, unsigned int>>& Nodes, const int DoF, const double a, const double b, const int m, const int n) {
    Nodes.reserve(DoF);            
        unsigned int k = 1;
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
Assembly (Eigen::MatrixXd& GSM, const int& DoF, const std::vector<std::pair<T, unsigned int>>& FEs) {
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


// Узловые перемещения
// Eigen::VectorXd Ndl_Dsplcmnts(const Eigen::MatrixXd& GSM, std::vector<std::tuple<int, bool, bool>> LBC_dof, std::vector<std::tuple<int, double, double>> LBC_force) { 
//             // Заполнение вектора нагрузок          
//             Eigen::VectorXd F = Eigen::VectorXd::Zero(GSM.rows()); // Вектор узловых нагрузок
//             for (auto& nd_force : LBC_force) {
//                 int& nbr = std::get<0>(nd_force);
//                 F[2 * (nbr - 1)] = std::get<1>(nd_force);
//                 F[2 * (nbr - 1) + 1] = std::get<2>(nd_force);
//             }

//             // Закрепление узлов в матрице жесткости
//             auto K = GSM;
//             for (auto& nd_dof : LBC_dof) {
//                 int& nbr = std::get<0>(nd_dof);
//                 if (std::get<1>(nd_dof)) {
//                     K.row(2 * (nbr - 1)) = Eigen::RowVectorXd::Zero(GSM.rows());
//                     K.col(2 * (nbr - 1)) = Eigen::VectorXd::Zero(GSM.rows());
//                     K(2 * (nbr - 1), 2 * (nbr - 1)) = 1;
//                 }
//                 if (std::get<2>(nd_dof)) {
//                     K.row(2 * (nbr - 1) + 1) = Eigen::RowVectorXd::Zero(GSM.rows());
//                     K.col(2 * (nbr - 1) + 1) = Eigen::VectorXd::Zero(GSM.rows());
//                     K(2 * (nbr - 1) + 1, 2 * (nbr - 1) + 1) = 1;
//                 }
//             }
//             //return K.colPivHouseholderQr().solve(F);
//             return K.partialPivLu().solve(F);
// }

Eigen::VectorXd Ndl_Dsplcmnts(const Eigen::MatrixXd& GSM, LBC LBC) { 
            // Заполнение вектора нагрузок          
            Eigen::VectorXd F = Eigen::VectorXd::Zero(GSM.rows()); // Вектор узловых нагрузок
            for (auto& nd_force : LBC.Forces) {
                int& nbr = std::get<0>(nd_force);
                F[2 * (nbr - 1)] = std::get<1>(nd_force);
                F[2 * (nbr - 1) + 1] = std::get<2>(nd_force);
            }

            // Закрепление узлов в матрице жесткости
            auto K = GSM;
            for (auto& nd_dof : LBC.DOF) {
                int& nbr = std::get<0>(nd_dof);
                if (std::get<1>(nd_dof)) {
                    K.row(2 * (nbr - 1)) = Eigen::RowVectorXd::Zero(GSM.rows());
                    K.col(2 * (nbr - 1)) = Eigen::VectorXd::Zero(GSM.rows());
                    K(2 * (nbr - 1), 2 * (nbr - 1)) = 1;
                }
                if (std::get<2>(nd_dof)) {
                    K.row(2 * (nbr - 1) + 1) = Eigen::RowVectorXd::Zero(GSM.rows());
                    K.col(2 * (nbr - 1) + 1) = Eigen::VectorXd::Zero(GSM.rows());
                    K(2 * (nbr - 1) + 1, 2 * (nbr - 1) + 1) = 1;
                }
            }
            //return K.colPivHouseholderQr().solve(F);
            return K.partialPivLu().solve(F);
        }

//Вектор деформаций в каждой точке треугольного КЭ при ПНС
template <typename T>
typename std::enable_if<std::is_same<T, TriangleFE>::value, Eigen::Vector3d>::type 
Strain(const Eigen::VectorXd& Vertice_displacements, const std::pair<T, unsigned int>& FE) {
    return FE.first.B() * Vertice_displacements;
}

//Вектор напряжений в каждой точке треугольного КЭ при ПНС
template <typename T>
typename std::enable_if<std::is_same<T, TriangleFE>::value, Stress2d>::type 
Stress(const Eigen::VectorXd& Vertice_displacements, const std::pair<T, unsigned int>& FE) {
    Eigen::Vector3d strs = FE.first.Mat().D() * Strain(Vertice_displacements, FE);
    return Stress2d({strs[0], strs[1], strs[2]});
}



// Полное решение задачи (Узловые перемещения, векторы деформаций и напряжений для всех КЭ при ПНС)
template <typename T>
typename std::enable_if<std::is_same<T, Plate_triangulated>::value, Solution>::type
solve(T& Plate, const LBC LBC) {           
    Eigen::VectorXd nodal_displacements = Ndl_Dsplcmnts(Plate.GSM(), LBC);
    std::vector<std::pair<Eigen::Vector3d, unsigned int>> strains; // Вектор деформаций (в любой точке) каждого КЭ
    std::vector<std::pair<Stress2d, unsigned int>> stresses; // Вектор напряжений (в любой точке) каждого КЭ
    strains.reserve(Plate.FEs().size());
    stresses.reserve(Plate.FEs().size());

    int NDoF = 6;
    // if constexpr (std::is_same<T, Plate_triangulated>::value) {
    //     NDoF = 6;
    // }      
    
    for (const auto& FE : Plate.FEs()) {
        // Получение перемещений вершин КЭ       
        Eigen::VectorXd vertices_displacements(NDoF); // Узловые перемещения КЭ (ЛВУОП)
        double* v_d = vertices_displacements.data(); // Указатель на первый элемент ЛВУОП
        for (const auto& node : FE.first.verts()) {
            int* nbr = new int(node.second);                
            *v_d = nodal_displacements[2 * (*nbr - 1)];
            ++v_d;
            *v_d = nodal_displacements[2 * (*nbr - 1) +1];
            ++v_d;
            delete nbr;
        }

        strains.emplace_back(std::make_pair(Strain(vertices_displacements, FE), FE.second));
        stresses.emplace_back(std::make_pair(Stress(vertices_displacements, FE), FE.second));
    }
                    
    return Solution(nodal_displacements, strains, stresses);
};

