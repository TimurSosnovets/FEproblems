#include  "../../headers/mechanical/Plate_types.hpp"
#include "../../headers/mechanical/Plate_methods.hpp"
#include <iostream>

Plate_triangulated::Plate_triangulated(const double lenX, const double lenY, const int m, const int n, const double thick, const Isotropic mater):
 _sideX(lenX), _sideY(lenY), _material(mater), _h(thick) {
    _DoF = 2 * (m+1) * (n+1);

    // Создание узлов
    Nodes_creation(_nodes, _DoF, _sideX, _sideY, m, n);

    // Создание КЭ
    int k = 1;
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {                    
            std::pair<Point, int>* p_ptr = &_nodes [i * (n+1) + j]; // Указатель на нижний левый узел квадрата
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

std::vector<std::pair<TriangleFE, int>> Plate_triangulated::FEs() const {
    return _finite_els;
}

std::vector<std::pair<Point, int>> Plate_triangulated::NDs() const {
    return _nodes;
}

Eigen::MatrixXd Plate_triangulated::GSM() const {
    return _global_SM;
}

double Plate_triangulated::thickness() const {
    return _h;
}

