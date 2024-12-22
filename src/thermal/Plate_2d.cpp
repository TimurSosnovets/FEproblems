#include "../../headers/thermal/Plate_2d.hpp"

// Разбиение на узлы
void Plate_2d::nodes_creation(const int m, const int n)
{
    _Nodes.reserve((m + 1) * (n + 1));
    int k = 0; // Счетчик номера узла
    // Вдоль ОХ
    for (int i = 0; i < m + 1; ++i) 
    {
       // Вдоль ОУ
       for (int j = 0; j < n + 1; ++j)
       {
            _Nodes.emplace_back() = {Point(i * (L_x / m), j * (L_y / n)), k + 1};
            ++k;
       } 
    }
    std::cout << "\n Number of nodes: " << k;
};

// Создание элементов
void Plate_2d::elements_creation(const int m, const int n)
{   
    std::function<int(int, int)> k;
    k = [this, n](int i, int j) 
                { return i * (n + 1) + j; }; // количество эл-тов вдоль слоя + 1;
    
    int n_elem = 0; // Счётчик номера элемента
    
    for (int i = 0; i < m; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            _FEs[n_elem] = {LQuad({_Nodes[k(i, j)], _Nodes[k(i+1, j)], _Nodes[k(i+1, j+1)], _Nodes[k(i, j+1)]}, mat.get_TCC(300), mat.get_TCC(300)), n_elem + 1};
            ++n_elem;
        }
    }
    std::cout << "\n Number of elements: " << n_elem;
};

// Конструктор класса
Plate_2d::Plate_2d(const Material material, const double length_x, const double length_y, const int quant_x, const int quant_y)
: mat(material), L_x(length_x), L_y(length_y), _DOF((quant_x + 1) * (quant_y + 1)) 
{
    nodes_creation(quant_x, quant_y);
    for (const auto& node : _Nodes)
    {
        std::cout << "\n Node " << node.second << ": x = " << node.first.x << ", y = " << node.first.y << ".";
    }
    std::cout << std::endl;

    elements_creation(quant_x, quant_y);
    for (const auto& elem : _FEs)
    {
        if (!elem.has_value()) break;
        std::cout << "\n Element " << elem.value().second << " nodes: ";
        for (const auto& vert : elem.value().first.Vertices())
        {
            std::cout << vert.get().second << " ";
        } 
        std::cout << "." << std::endl;
    }

    Assembly<LQuad, MAX_DOF>(_GCM, _DOF, &_FEs);
};



// Возвращаемые значения
const std::vector<std::pair<Point, int>>& Plate_2d::Nodes() const
{
    return _Nodes;
}

const std::array<std::optional<std::pair<LQuad, int>>, MAX_DOF>& Plate_2d::FEs() const
{
    return _FEs;
}

const Eigen::MatrixXd& Plate_2d::GCM() const
{
    return _GCM;
}