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
        Eigen::VectorXd Dynamic_calculation(Eigen::VectorXd initial_temp, double max_time, double time_step) const; // Решение нестационарной задачи с заданными начальными условиями, временем расчёта и шагом.

    public:
        /*Конструктор класса*/
        TFE_model(size_t dx, size_t dy, size_t dz); // Инициализация сетки с заданным количеством элементов по каждому направлению

        /*Добавление структурных единиц*/
        void add_node(Point p, int g_nbr);
        void add_element(std::vector<Node*> verts, int g_nbr, const Material* material, bool is_surf = false, double surf_area = 0, std::string* layer = nullptr,  std::string* primitive = nullptr);

        /*Предрасчёт сетки*/
        void pre_calculate();

        /*Вычисление параметров*/
        Eigen::MatrixXd GCM(Eigen::VectorXd nodal_temps) const; // Глобальная матрица теплопроводности
        Eigen::MatrixXd GDM(Eigen::VectorXd nodal_temps) const; // Глобальная матрица демпфирования
        Eigen::VectorXd F(double q, double eps, Eigen::VectorXd nodal_temps) const; // Вектор узловых нагрузок

        /*Вывод информации*/
        void mesh_info() const;
};
