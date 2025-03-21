#include "FE_entities.hpp"
#include "LQube_IP.hpp"
#include "LWedge_IP.hpp"

/*Узел*/
Node::Node() : point(), gn(0) {} // Дефолтный конструктор
Node::Node(double x, double y, int n) : point(Point(x,y)), gn(n) {} // 2d конструктор
Node::Node(double x, double y, double z, int n) : point(Point(x,y,z)), gn(n) {} // 3d конструктор
Node::Node(Point p, int n) : point(p), gn(n) {} // точка-конструктор
// Номер узла
int Node::global_number() const {return gn;}
// Координаты
const Point& Node::coords() const {return point;}

/*Конечный элемент*/
//Конструктор
Element::Element(std::unique_ptr<Isoparametric_3D> fe_type, const std::vector<const Node*>& v, const int n, const Material* const m, const bool s, const float area) : 
type(std::move(fe_type)), 
vertices(v), 
gn(n), 
material(m), 
is_surface(s), 
surface_area(area) 
{}
// Указание слоя
void Element::set_layer(std::string* lr) {layer = lr;}
// Указание геометрического примитива
void Element::set_primitive(std::string* pr) {primitive = pr;}
// Наличие предрасчитанных значений
bool Element::has_cache() const {return !cache.GM.empty();}
// Информация об элементе
void Element::get_info(const bool to_console, const std::string& filename) const
{   
    /*Инициализация*/
    std::string message, m_type, m_surface, m_layer, m_primitive, nodes;
        
    /*Определение качественных характеристик*/
        // Тип элемента 
    if (type) 
    {
        if (dynamic_cast<LQube*>(type.get())) {m_type = "LQube";}
        else if (dynamic_cast<LWedge*>(type.get())) {m_type = "LWedge";}
        else {m_type = "Unknown Type";}
    } 
    else {m_type = "Not assigned";}
        // Является ли поверхностным
    m_surface = is_surface ? "surface" : "internal";
        // В каком слое находится
    m_layer = (layer == nullptr) ? "not assigned" : *layer;
        // Частью какой геометрии является
        m_primitive = (primitive == nullptr) ? "not assigned" : *primitive;      
    /*Массив номеров узлов через пробел*/
    for (size_t i = 0; i < vertices.size(); ++i)
    {
        nodes += std::to_string(vertices[i]->global_number());
        if (i < vertices.size() - 1) { nodes += " "; }
    }

    /*Непосредственно сообщение*/
    message = "Element " + std::to_string(gn) + ": " + m_type + ", " + m_surface + ", layer - " + m_layer + ", primitive - " + m_primitive + ", nodes {" + nodes + "}.\n";
        
    /*Вывод сообщения*/
    logger::log(message, to_console, filename);
}




