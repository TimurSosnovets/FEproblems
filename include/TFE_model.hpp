#pragma once
// Current project
#include "Structs.hpp"
#include "Materials.hpp"
// Eigen
#include <Dense>


// Тепловая конечно-элементная модель
class TFE_model
{
    private:
        /*Параметры класса*/
        std::vector<Node> _nodes; // Массив узлов
        std::vector<Element> _elements; // Массив элементов
        const size_t _DOF; // Степень свободы модели (в данном случае оно же - количество узлов)

        /*Внутренние методы*/
        void assembly(Eigen::MatrixXd A, Eigen::MatrixXd a) const; // Ассамблирование матрицы A размерности [DOF x DOF] из меньшей матрицы a

    public:
        /*Конструктор класса*/
        TFE_model(size_t dx, size_t dy, size_t dz); // Инициализация сетки с заданным количеством элементов по каждому направлению

        /*Добавление структурных единиц*/
        void add_node(Point p, int g_nbr) const;
        void add_element(std::vector<Node*> verts, int g_nbr, Material& mat, bool is_surf = false, double surf_area = 0, std::string* layer = nullptr,  std::string* primitive = nullptr) const;
};
