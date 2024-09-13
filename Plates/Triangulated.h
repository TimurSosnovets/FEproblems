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
           
            std::sort(_vertices.begin(), _vertices.end(), compare_by_x);
            
            std::vector<double*> X, Y; // Массив указалтелей на координаты точек
            for (const auto& node : _vertices) {
                X.push_back(new double(node.first.coord_x()));
                Y.push_back(new double(node.first.coord_y()));
            }

            double Delta = (*X[1] * *Y[2]) - (*X[2] * *Y[1]) - (*X[0] * *Y[2]) + (*X[2] * *Y[0]) + (*X[0] * *Y[1]) - (*X[1] * *Y[0]); // Определитель матрицы системы (по Крамеру)
           
            _B << *Y[1]-*Y[2], 0, *Y[2]-*Y[0], 0, *Y[0]-*Y[1], 0,
                  0, *X[2]-*X[1], 0, *X[0]-*X[2], 0, *X[1]-*X[2],
                  *X[2]-*X[1], *Y[1]-*Y[2], *X[0]-*X[2], *Y[2]-*Y[0], *X[1]-*X[2], *Y[0]-*Y[1];
            _B = _B / Delta;

            _K = 1/2 * Delta * _h * _B.transpose() * _material.D() * _B;

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
        std::vector<std::pair<Displacement2d, unsigned int>> _nodal_disp; // Узловые перемещения и номер узла 
        std::vector<std::pair<TriangleFE, unsigned int>> _finite_el; // Конечные элементы и их номера
        unsigned int _DoF; // Степени свободы (размер глобальной матрицы жесткости)
        Eigen::MatrixXd _global_SM; // Глобальная матрица жесткости

    public:
        Plate_triangulated(const double lenX, const double lenY, const unsigned int m, const unsigned int n, Material mater): _sideX(lenX), _sideY(lenY), _material(mater) {
            _DoF = 2 * (m+1) * (n+1);

            // Создание узлов
            _nodes.reserve(_DoF);
            _nodal_disp.reserve(_DoF);
            unsigned int k = 1;
            for (int i = 0; i < m+1; ++i) {
                for (int j = 0; j < n+1; ++j) {
                    _nodes.emplace_back(std::make_pair(Point(_sideX/m * i, _sideY/n * j), k));
                    _nodal_disp[k].second = k;
                    k++;
                }
            }

            // Создание КЭ
            k = 1;
             for (int i = 0; i < m; ++i) {
                for (int j = 0; j < n; ++j) {
                    // unsigned int t = i * (n+1) + j;
                    // _finite_el.emplace_back(std::make_pair(TriangleFE({_nodes[t], _nodes[t+1], _nodes[t+1+n]}, _material, _h), k));
                    // k++;
                    // _finite_el.emplace_back(std::make_pair(TriangleFE({_nodes[t+1], _nodes[t+2+n], _nodes[t+1+n]}, _material, _h), k));
                    // k++;
                    std::pair<Point, unsigned int>* p_ptr = &_nodes [i * (n+1) + j]; // Указатель на нижний левый узел квадрата
                     _finite_el.emplace_back(std::make_pair(TriangleFE({*p_ptr, *(p_ptr + 1), *(p_ptr + n + 1)}, _material, _h), k));
                     k++;
                    _finite_el.emplace_back(std::make_pair(TriangleFE({*(p_ptr + 1), *(p_ptr + n + 1), *(p_ptr + n + 2)}, _material, _h), k));
                    k++;
                }
            }
        }
};