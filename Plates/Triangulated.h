#pragma once
#include "PSS_Vectors.h"
#include "FEtypes/Triangle_2dof.h"
#include "Methods.h"
#include <iostream>

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
        Plate_triangulated(const double lenX, const double lenY, const unsigned int m, const unsigned int n, const double thick, const Material mater): _sideX(lenX), _sideY(lenY), _material(mater), _h(thick) {
            _DoF = 2 * (m+1) * (n+1);

            // Создание узлов
           Nodes_creation(_nodes, _DoF, _sideX, _sideY, m, n);

            // Создание КЭ
            int k = 1;
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
            Assembly(_global_SM, _DoF, _finite_els);

            std::cout << "Object successfully created: node count - " << _nodes.size() << "; number of elements - " << _finite_els.size() << ".\n";
        }
        
        std::vector<std::pair<TriangleFE, unsigned int>> FEs() const {
            return _finite_els;
        }

        std::vector<std::pair<Point, unsigned int>> NDs() const {
            return _nodes;
        }

        Eigen::MatrixXd GSM() const {
            return _global_SM;
        }

        double thickness() const {
            return _h;
        }

};