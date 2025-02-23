//Current project
#include "TFE_model.hpp"
#include "Output.hpp"

// Конструктор класса
TFE_model::TFE_model(const size_t dx, const size_t dy, const size_t dz) : _DOF((dx+1) * (dy+1) * (dz+1))
{
    _nodes.reserve(_DOF);
    _elements.reserve(dx * dy * dz);
}

// Информация о сетке
void TFE_model::mesh_info() const
{
    /*Инициализация*/
    std::string message;
    std::string filename;
    bool to_console;
    if (_elements.size() > 30) {filename = "log.txt"; to_console = false;}
    else {filename = ""; to_console = true;}

    /*Общая информация*/
    message = "TFE model: " + std::to_string(_nodes.size()) + " nodes, " + std::to_string(_elements.size()) + " elements.\n";
    logger::log(message, to_console, filename);

    /*Вывод информации по элементам*/
    message = "/n/n/n============/nElement info/n============/n";
    logger::log(message, to_console, filename);
    for (const auto& element : _elements)
    {
        element.get_info(to_console, filename);
    }
}

// Добавление узла
void TFE_model::add_node(const Point p, const int g_nbr)
{
    _nodes.emplace_back(p, g_nbr);
}

// Добавление элемента
void TFE_model::add_element(const std::vector<Node*>& verts, const int& g_nbr, const Material* const material, const bool is_surf = false, const float& surf_area = 0, std::string* const layer = nullptr, std::string* const primitive = nullptr)
{
    auto& new_element = _elements.emplace_back(verts, g_nbr, material, is_surf, surf_area);
    if (layer) {new_element.set_layer(layer);}
    if (primitive) {new_element.set_primitive(primitive);}

}

// Предрасчёт элемента
void TFE_model::pre_calculate()
{
    for (const auto& element : _elements)
    {
        element.
    }
}



