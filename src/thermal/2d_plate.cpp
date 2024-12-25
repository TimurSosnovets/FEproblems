#include "../../headers/thermal/2d_plate.hpp"
#include <iostream>


// Слои
enum Layer {GLASSY_CARBON, TZMK, AMG}; // Итерации по слоям

// Разбиение на узлы
void LPlate::nodes_creation(const int W, const std::array<int, 3> L_h)
{
    _nodes.reserve(_DOF);
    int k = 1; // Номер узла
    double x, y; // Координаты точки

    const double step_W = width / W; // Шаг по ширине
    std::array<double, 3> step_layer; // Шаг по слоям
    for (int l = 0; l < 3; ++l)
    {
        if (hight[l] == 0) {step_layer[l] = 0.0;}
        else {step_layer[l] = hight[l] / L_h[l];}
    }

    for (int i_W = 0; i_W < W + 1; ++i_W)
    {   
        x = i_W * step_W;
        Layer CurrentL = GLASSY_CARBON;
        int i_h = 0; // Счётчик по слою

        while (i_h < std::accumulate(L_h.begin(), L_h.end(), 0) + 1)
        {
            switch (CurrentL) 
            {                
                case GLASSY_CARBON:
                    if (L_h[0] == 0) {CurrentL = TZMK; continue;}
                    y = i_h * step_layer[0];
                    if (i_h == L_h[0]) {CurrentL = TZMK;}
                break;

                case TZMK:
                    if (L_h[1] == 0) {CurrentL = AMG; continue;}
                    y = hight[0] + (i_h - L_h[0]) * step_layer[1];
                    if (i_h == L_h[0] + L_h[1]) {CurrentL = AMG;}
                break;

                case AMG:
                    if (L_h[2] == 0) {break;}
                    y = hight[0] + hight[1] + (i_h - L_h[0] - L_h[1]) * step_layer[2];
                break;
            }  
            _nodes.emplace_back(x, y, k);
            ++k;
            ++i_h;
        }
        i_h = 0;
    }
    std::cout << "\n Number of nodes: " << k - 1;
};

// Создание элементов 
void LPlate::elements_creation(const int W, const std::array<int, 3> L_h)
{
    _elements.reserve(W * std::accumulate(L_h.begin(), L_h.end(), 0));

    // Глобальный номер левой нижней вершины элемента
    std::function<int(int, int)> k;
    k = [this, L_h](int i, int j) { return i * (std::accumulate(L_h.begin(), L_h.end(), 0) + 1) + j; }; // количество эл-то вдоль слоя + 1

    int n = 1;
    const Material* material; // Материал элемента
    bool is_surface; // Маркер поверхностного элемента
    // Заполнение массива эелементов
    for (int i = 0; i < W; ++i)
    {
        Layer CurrentL = GLASSY_CARBON;
        is_surface = true;
        int j = 0; // Счётчик по слою

        while (j < std::accumulate(L_h.begin(), L_h.end(), 0))
        {
            switch (CurrentL) 
            {                
                case GLASSY_CARBON:
                    if (L_h[0] == 0) {CurrentL = TZMK; continue;}
                    material = &GC_2500;
                    if (j == L_h[0]-1) {CurrentL = TZMK;}
                break;

                case TZMK:
                    if (L_h[1] == 0) {CurrentL = AMG; continue;}
                    material = &TZMK_10;
                    if (j == L_h[0] + L_h[1]-1) {CurrentL = AMG;}
                break;

                case AMG:
                    material = &AMg_6;
                break;
            }
            std::vector<Node*> vertices = {&_nodes[k(i,j)], &_nodes[k(i+1,j)], &_nodes[k(i+1,j+1)], &_nodes[k(i,j+1)]};
            _elements.emplace_back(vertices, n, *material, is_surface);
            is_surface = false;
            ++n;
            ++j;  
        }
    }
}


// Конструктор класса
LPlate::LPlate(const std::array<Material, 3> Layer_materials, const double Width, const std::array<double, 3> Layer_hights, const int FEC_width, const std::array<int, 3> FEC_layer)
: width(Width), hight(Layer_hights), materials(Layer_materials), _DOF((FEC_width + 1) * (std::accumulate(FEC_layer.begin(), FEC_layer.end(), 0) + 1)) 
{
    nodes_creation(FEC_width, FEC_layer);
    // Тестовый вывод
    for (const auto& node : _nodes)
    {
        std::cout << "\n Node "<< node.gn << " " <<  ": x = " << node.point.x << ", y = " << node.point.y << ".";
    }
    std::cout << std::endl;

    elements_creation(FEC_width, FEC_layer);
    // Тестовый вывод
    for (const auto& element : _elements)
    {
        std::cout << "\n Element "<< element.gn << " nodes:";
        for (const auto& node : element.vertices) {std::cout << " " << node->gn;}
        std::cout << "; lambda = " << element.material.get_TCC(300) << "; is surfase?: " << element.is_surface << " ; ";
    }
    std::cout << std::endl;
};

// Глобальная матрица теплопроводности 
Eigen::MatrixXd LPlate::GCM(Eigen::VectorXd nodal_temps) const
{   
    Eigen::MatrixXd GCM = Eigen::MatrixXd::Zero(_DOF, _DOF);
    Eigen::Matrix<double, 4, 4> H;
    Eigen::Vector<double, 4> T;
    for (const auto& element : _elements)
    {   
        // Заполняем локальный вектор узловых температур
        int i = 0;
        for (const auto& node : element.vertices)
        {
            T[i] = nodal_temps[node->gn - 1];
            ++i;
        }

        // Создаём конечный элемент
        LQuad Quad(element.vertices, element.material);
        H = Quad.Cond_Mat(T); 

        // Переносим значения в глобальную матрицу
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                int row = element.vertices[i]->gn - 1;
                int col = element.vertices[j]->gn - 1;
                GCM(row, col) += H(i, j);
            }
        }
    }

    return GCM;
}


// Глобальная матрица демпфирования 
Eigen::MatrixXd LPlate::GDM(Eigen::VectorXd nodal_temps) const
{   
    Eigen::MatrixXd GDM = Eigen::MatrixXd::Zero(_DOF, _DOF);
    Eigen::Matrix<double, 4, 4> C;
    Eigen::Vector<double, 4> T;
    for (const auto& element : _elements)
    {   
        // Заполняем локальный вектор узловых температур
        int i = 0;
        for (const auto& node : element.vertices)
        {
            T[i] = nodal_temps[node->gn - 1];
            ++i;
        }

        // Создаём конечный элемент
        LQuad Quad(element.vertices, element.material);
        C = Quad.Damp_Mat(T); 

        // Переносим значения в глобальную матрицу
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                int row = element.vertices[i]->gn - 1;
                int col = element.vertices[j]->gn - 1;
                GDM(row, col) += C(i, j);
            }
        }
    }

    return GDM;
}

// Глобальный вектор узловых нагрузок
Eigen::VectorXd LPlate::F(const double q, const double eps, Eigen::VectorXd nodal_temps) const
{
    Eigen::VectorXd Fv = Eigen::VectorXd::Zero(_DOF);
    Eigen::Vector<double, 4> f;
    Eigen::Vector<double, 4> T;

    for (const auto& element : _elements)
    {   
        if (!element.is_surface) {continue;}

        // Заполняем локальный вектор узловых температур
        int i = 0;
        for (const auto& node : element.vertices)
        {
            T[i] = nodal_temps[node->gn - 1];
            ++i;
        }

        // Создаём конечный элемент
        LQuad Quad(element.vertices, element.material);
        f = Quad.Heat_Load_Surf(q, eps, T, {0, 1}); 

        // Переносим значения в глобальную матрицу
        for (int j = 0; j < 4; ++j)
        {
            int row = element.vertices[j]->gn - 1;
            Fv[row] += f[j];
        }
    }

    return Fv;
} 

// Возвращаемые значения
const std::vector<Node>& LPlate::Nodes() const
{
    return _nodes;
}

const std::vector<Element>& LPlate::Elements() const
{
    return _elements;
}
