#pragma once
#include "Material_properties.h"
#include "../../2dGeometry/2dPoint.h"



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

        Material Mat() const {
            return _material;
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