#pragma once
// Current project
#include "Structs.hpp"
#include "Materials.hpp"
// Eigen
#include <Dense>
#include <Sparse>
#include <Core>


// Тепловая конечно-элементная модель
class TFE_model
{
    private:
        /*Параметры класса*/
        std::vector<Node> _nodes; // Массив узлов
        std::vector<Element> _elements; // Массив элементов
        const size_t _DOF; // Степень свободы модели (в данном случае оно же - количество узлов)

        /*Внутренние методы*/
        void assembly(Eigen::SparseMatrix<double> A, const Eigen::MatrixXd a) const; // Ассамблирование матрицы A размерности [DOF x DOF] из меньшей матрицы a
        Eigen::VectorXd Dynamic_calculation(const float initial_temp, const int max_time, const float time_step) const; // Решение нестационарной задачи с заданными начальными условиями, временем расчёта и шагом.

    public:
        /*Конструктор класса*/
        TFE_model(const size_t dx, const size_t dy, const size_t dz); // Инициализация сетки с заданным количеством элементов по каждому направлению

        /*Добавление структурных единиц*/
        void add_node(const Point p, const int g_nbr);
        void add_element(const std::vector<Node*>& verts, const int& g_nbr, const Material* const material, const bool is_surf = false, const float& surf_area = 0, const std::string* const layer = nullptr, const std::string* const primitive = nullptr);

        /*Предрасчёт сетки*/
        void pre_calculate();

        /*Вычисление параметров*/
        Eigen::SparseMatrix<double> GCM(const Eigen::VectorXd nodal_temps) const; // Глобальная матрица теплопроводности
        Eigen::SparseMatrix<double> GDM(const Eigen::VectorXd nodal_temps) const; // Глобальная матрица демпфирования
        Eigen::SparseVector<double> F(const double q, const double eps, const Eigen::VectorXd nodal_temps) const; // Вектор узловых нагрузок

        /*Вывод информации*/
        void mesh_info() const;
};
