#pragma once
#include "FEtypes/Triangle_2dof.h"
#include "Structs.h"

class Plate_triangulated;
class Point;
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

// Получение узловых перемещений
template <typename T>
typename std::enable_if<std::is_same<T, Plate_triangulated>::value, Eigen::VectorXd>::type
Ndl_Dsplcmnts(const T& Plate, LBC LBC) { 
            // Заполнение вектора нагрузок          
            Eigen::VectorXd F = Eigen::VectorXd::Zero(Plate.GSM().rows()); // Вектор узловых нагрузок
            for (auto& nd_force : LBC.Forces) {
                int& nbr = std::get<0>(nd_force);
                F[2 * (nbr - 1)] = std::get<1>(nd_force);
                F[2 * (nbr - 1) + 1] = std::get<2>(nd_force);
            }

            // Вектор узловых сил от температурных деформаций
            double NTemp = 293; // Температура при НУ
            Eigen::VectorXd Temp = Eigen::VectorXd::Zero(Plate.GSM().rows()); // Вектор относительных температур элементов
             // Вытаскиваем узловые температуры
            int n = 3;
            for (const auto& FE : Plate.FEs()) {
                for (const auto& Ndl_T : LBC.Nodal_Temp) {
                    auto it = std::find_if(FE.first.verts().begin(), FE.first.verts().end(), [Ndl_T] (const std::pair<Point, unsigned int>& vert) {
                        return vert.second == Ndl_T.second;
                    });
                    if (it != FE.first.verts().end()) {
                        Temp[(*it).second - 1] += (Ndl_T.first - NTemp) / n;
                    }    
                }
            }
             // Вытаскиваем температуры элементов
            for (const auto& Elt_T : LBC.Element_Temp) {
                if (Temp[Elt_T.second - 1] < (Elt_T.first - NTemp)) {
                    Temp[Elt_T.second - 1] = Elt_T.first - NTemp;
                }
            }
             // Переводим температуру элемента в узловые силы
            Eigen::Vector3d Temp_Strain;
            Eigen::VectorXd Temp_Forces(6);
            for (unsigned int i = 0; i = Plate.FEs().size() - 1; ++i) {
                double a = Plate.FEs()[i].first.Mat().CLTE(); // КЛТР
                Temp_Strain << a * Temp[i], a * Temp[i], 0;
                Temp_Forces = (-Plate.thickness()) * (Plate.FEs()[i].first.Square()) * (Plate.FEs()[i].first.B().transpose() * Plate.FEs()[i].first.Mat().D() * Temp_Strain);
                for (int j = 0; j = 2; ++j) {
                    F[2 * (Plate.FEs()[i].first.verts()[j].second - 1)] += Temp_Forces[2 * j];
                    F[2 * (Plate.FEs()[i].first.verts()[j].second - 1) + 1] += Temp_Forces[2 * j + 1];
                }
            }
            // Закрепление узлов в матрице жесткости
            auto K = Plate.GSM();
            for (auto& nd_dof : LBC.DOF) {
                int& nbr = std::get<0>(nd_dof);
                if (std::get<1>(nd_dof)) {
                    K.row(2 * (nbr - 1)) = Eigen::RowVectorXd::Zero(Plate.GSM().rows());
                    K.col(2 * (nbr - 1)) = Eigen::VectorXd::Zero(Plate.GSM().rows());
                    K(2 * (nbr - 1), 2 * (nbr - 1)) = 1;
                }
                if (std::get<2>(nd_dof)) {
                    K.row(2 * (nbr - 1) + 1) = Eigen::RowVectorXd::Zero(Plate.GSM().rows());
                    K.col(2 * (nbr - 1) + 1) = Eigen::VectorXd::Zero(Plate.GSM().rows());
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
    Eigen::VectorXd nodal_displacements = Ndl_Dsplcmnts(Plate, LBC);
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

