#pragma once
#include "PSS_Vectors.h"
#include "Material_properties.h"
#include "..\2dGeometry\2dPoint.h"


// Треугольный конечный элемент
class TriangleFE {
    private:
        std::vector<std::pair<Point, unsigned int>> _vertices; // Вершины и глобальная нумерация
        Eigen::Matrix<double, 6, 6> _K; // Матрица жесткости элемента
        Eigen::Matrix<double, 3, 6> _B; // Матрица функций деформаций 
        Material _material; // Материал элемента
        double _h; // Толщина

    public:
        std::vector<std::pair<Point, unsigned int>> verts() const {
            return _vertices;
        }

        Eigen::Matrix<double, 6, 6> K() const {
            return _K;
        }

        Eigen::Matrix<double, 3, 6> B() const {
            return _B;
        }

        
        TriangleFE(const std::vector<std::pair<Point, unsigned int>> Vertices, const Material material, const double h): _vertices(Vertices), _material(material), _h(h) {
            if (Vertices.size() != 3) throw std::invalid_argument("The triangle must have exactly 3 vertices");
           
            
            
            std::vector<double*> X, Y; // Массив указалтелей на координаты точек
            for (const auto& node : _vertices) {
                X.push_back(new double(node.first.coord_x()));
                Y.push_back(new double(node.first.coord_y()));
            }

            double Delta = (*X[1] * *Y[2]) - (*X[2] * *Y[1]) - (*X[0] * *Y[2]) + (*X[2] * *Y[0]) + (*X[0] * *Y[1]) - (*X[1] * *Y[0]); // Определитель матрицы системы (по Крамеру)
            // Проверяем ориентацию треугольника
            if (Delta == 0) {
                throw std::runtime_error("Degenerate triangle (points are collinear).");
            }
            // При необходимости переориентируем его против часовой стрелки
            if (Delta < 0) {
                std::swap(_vertices[1], _vertices[2]);
                Delta = -Delta;
                // Удаляем указатели с неверной ориентацией
                for (unsigned int i = 0; i < 3; ++i) {
                    delete X[i];
                    delete Y[i];
                }
                X.clear();
                Y.clear();
                // Создаем указатели с верной ориентацией                
                for (const auto& node : _vertices) {
                    X.push_back(new double(node.first.coord_x()));
                    Y.push_back(new double(node.first.coord_y()));
                }
            }


            _B << *Y[1]-*Y[2], 0, *Y[2]-*Y[0], 0, *Y[0]-*Y[1], 0,
                  0, *X[2]-*X[1], 0, *X[0]-*X[2], 0, *X[1]-*X[0],
                  *X[2]-*X[1], *Y[1]-*Y[2], *X[0]-*X[2], *Y[2]-*Y[0], *X[1]-*X[0], *Y[0]-*Y[1];
            _B = _B / Delta;

            _K = 0.5 * Delta * _h * (_B.transpose() * _material.D() * _B);
            

            for (unsigned int i = 0; i < 3; ++i) {
                delete X[i];
                delete Y[i];
            }

            X.clear();
            Y.clear();
        }
        
};

//Пластина, разбитая на треугольные КЭ
class Plate_triangulated {
    private:
        double _sideX; // Длина стороны, параллельной OX
        double _sideY; // Длина стороны, параллельной OY
        Material _material; // Материал пластины
        double _h; // Толщина пластины
        std::vector<std::pair<Point, unsigned int>> _nodes; // Узлы и их глобальная нумерация        
        std::vector<std::pair<TriangleFE, unsigned int>> _finite_els; // Конечные элементы и их номера
        unsigned int _DoF; // Степени свободы (размер глобальной матрицы жесткости)
        Eigen::MatrixXd _global_SM; // Глобальная матрица жесткости

    public:
        Plate_triangulated(const double lenX, const double lenY, const unsigned int m, const unsigned int n, Material mater): _sideX(lenX), _sideY(lenY), _material(mater) {
            _DoF = 2 * (m+1) * (n+1);

            // Создание узлов
            _nodes.reserve(_DoF);            
            unsigned int k = 1;
            for (int i = 0; i < m+1; ++i) {
                for (int j = 0; j < n+1; ++j) {
                    _nodes.emplace_back(std::make_pair(Point(_sideX/m * i, _sideY/n * j), k));                    
                    k++;
                }
            }

            // Создание КЭ
            k = 1;
             for (int i = 0; i < m; ++i) {
                for (int j = 0; j < n; ++j) {                    
                    std::pair<Point, unsigned int>* p_ptr = &_nodes [i * (n+1) + j]; // Указатель на нижний левый узел квадрата
                     _finite_els.emplace_back(std::make_pair(TriangleFE({*p_ptr, *(p_ptr + 1 + n), *(p_ptr + 1)}, _material, _h), k));
                     k++;
                    _finite_els.emplace_back(std::make_pair(TriangleFE({*(p_ptr + 1), *(p_ptr + n + 1), *(p_ptr + n + 2)}, _material, _h), k));
                    k++;
                }
            }

            // Ассамблирование
            _global_SM = Eigen::MatrixXd::Zero(_DoF, _DoF);

            std::vector<int> Gnn; // Вектор индексов коэффициентов в глобальной матрице
            Gnn.reserve(6);

            for (const auto& FE: _finite_els) {

                // Заполняем вектор (учитывается, что номера начинались с единицы)
                for (const auto& node : FE.first.verts()) {
                    Gnn.emplace_back(2 * (node.second - 1));
                    Gnn.emplace_back(2 * (node.second - 1) + 1);
                }

                // Заполнение глобальной матрицы
                double* K_ij = FE.first.K().data();
                for (const auto& colm : Gnn) {
                    for (const auto& row : Gnn) {
                        _global_SM (row, colm) = *K_ij;
                        ++K_ij;
                    }
                }

                Gnn.clear(); // Очищаем вектор
            }

            Gnn.shrink_to_fit();
        }

        // Узловые перемещения
        Eigen::VectorXd Ndl_Dsplcmnts(std::vector<std::tuple<int, bool, bool>> LBC_dof, std::vector<std::tuple<int, double, double>> LBC_force) { 
            // Заполнение вектора нагрузок          
            Eigen::VectorXd F = Eigen::VectorXd::Zero(_DoF); // Вектор узловых нагрузок
            for (auto& nd_force : LBC_force) {
                int& nbr = std::get<0>(nd_force);
                F[2 * (nbr - 1)] = std::get<1>(nd_force);
                F[2 * (nbr - 1) + 1] = std::get<2>(nd_force);
            }

            // Закрепление узлов в матрице жесткости
            auto K = _global_SM;
            for (auto& nd_dof : LBC_dof) {
                int& nbr = std::get<0>(nd_dof);
                if (std::get<1>(nd_dof)) {
                    K.row(2 * (nbr - 1)) = Eigen::RowVectorXd::Zero(_DoF);
                    K.col(2 * (nbr - 1)) = Eigen::VectorXd::Zero(_DoF);
                    K(2 * (nbr - 1), 2 * (nbr - 1)) = 1;
                }
                if (std::get<2>(nd_dof)) {
                    K.row(2 * (nbr - 1) + 1) = Eigen::RowVectorXd::Zero(_DoF);
                    K.col(2 * (nbr - 1) + 1) = Eigen::VectorXd::Zero(_DoF);
                    K(2 * (nbr - 1) + 1, 2 * (nbr - 1) + 1) = 1;
                }
            }

            return K.colPivHouseholderQr().solve(F);
        }

        // Вектор деформаций в каждой точке треугольного КЭ при ПНС 
        Eigen::Vector3d Strain(const Eigen::VectorXd& Vertice_displacements, const std::pair<TriangleFE, unsigned int>& FE) {
            return FE.first.B() * Vertice_displacements;
        }

        // Вектор напряжений в каждой точке треугольного КЭ при ПНС
        Stress2d Stress(const Eigen::Vector3d& Strain) {
            Eigen::Vector3d strs = _material.D() * Strain;
            return Stress2d({strs[0], strs[1], strs[2]});
        }

        // Полное решение задачи (Узловые перемещения, векторы деформаций и напряжений для всех КЭ при ПНС)
        std::tuple<Eigen::VectorXd, std::vector<std::pair<Eigen::Vector3d, unsigned int>>, std::vector<std::pair<Stress2d, unsigned int>>>
         solve(const std::vector<std::tuple<int, bool, bool>> LBC_dof, const std::vector<std::tuple<int, double, double>> LBC_force) {
           
            Eigen::VectorXd nodal_displacements = Ndl_Dsplcmnts(LBC_dof, LBC_force);
            std::vector<std::pair<Eigen::Vector3d, unsigned int>> strains; // Вектор деформаций (в любой точке) каждого КЭ
            std::vector<std::pair<Stress2d, unsigned int>> stresses; // Вектор напряжений (в любой точке) каждого КЭ
            strains.reserve(_finite_els.size());
            stresses.reserve(_finite_els.size());
            
            for (const auto& FE : _finite_els) {
                // Получение перемещений вершин КЭ
                Eigen::VectorXd vertices_displacements(6); // Узловые перемещения КЭ (ЛВУОП)
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
                stresses.emplace_back(std::make_pair(Stress(strains[FE.second - 1].first), FE.second));
            }

            return std::make_tuple(nodal_displacements, strains, stresses);
        }

};