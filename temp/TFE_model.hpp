#pragma once
// Current project
#include "FE_entities.hpp"
#include "Materials.hpp"
#include "LQube_IP.hpp"
// Eigen
#include <Dense>
#include <Sparse>
#include <Core>
#include <unordered_set>

// Для вычисления уникальных комбинаций строка-столбец на базе КЭ сетки
struct PairHash 
{
    size_t operator()(const std::pair<int, int>& p) const {return std::hash<int>()(p.first) ^ std::hash<int>()(p.second);}
};

// Тепловая конечно-элементная модель
class TFE_model
{
    private:
        /*Параметры класса*/
        std::vector<Node> _nodes; // Массив узлов
        std::vector<Element> _elements; // Массив элементов
        const size_t _DOF; // Степень свободы модели (в данном случае оно же - количество узлов)
        size_t unique_DOF = 0; // Количество ненулевых значений в матрицах (зависит только от сетки)

        /*Внутренние методы*/
        void assembly(std::vector<Eigen::Triplet<double>>& t, const Eigen::MatrixXd& a, const Element& FE) const; // Ассамблирование матрицы A размерности [DOF x DOF] из меньшей матрицы a

    public:
        /*Конструктор класса*/
        TFE_model(const size_t dx, const size_t dy, const size_t dz); // Инициализация сетки с заданным количеством элементов по каждому направлению

        /*Добавление структурных единиц*/
        void add_node(const Point p, const int g_nbr);
        void add_element(const ElementType fe_type, const std::vector<Node*>& verts, const int& g_nbr, const Material* const material, const bool is_surf = false, const float& surf_area = 0, std::string* const layer = nullptr, std::string* const primitive = nullptr);

        /*Предрасчёт сетки*/
        void pre_calculate();
        void mesh_check();

        /*Вычисление параметров*/
        Eigen::SparseMatrix<double> GCM(const Eigen::VectorXd& nodal_temps) const; // Глобальная матрица теплопроводности
        Eigen::SparseMatrix<double> GDM(const Eigen::VectorXd& nodal_temps) const; // Глобальная матрица демпфирования
        Eigen::SparseVector<double> NLV(const double q, const double eps, const Eigen::VectorXd& nodal_temps) const; // Вектор узловых нагрузок

        /*Решение нестационарной задачи с заданными начальными условиями, временем расчёта и шагом.*/
        Eigen::VectorXd Dynamic_calculation(const float initial_temp, const int max_time, const float time_step) const; 

        /*Вывод информации*/
        void mesh_info() const;
};
