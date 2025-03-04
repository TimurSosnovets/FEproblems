#pragma once
// Current project
#include "FE_entities.hpp"
#include "Materials.hpp"
#include "LQube_IP.hpp"
#include "Output.hpp"
// Eigen
#include <IterativeLinearSolvers>
// STL
#include <unordered_set>
#include <chrono>

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
        size_t unique_DOF_surf = 0; // Количество ненулевых значений в векторе нагрузок (зависит только от сетки)

        /*Внутренние методы*/
        void assembly(std::vector<Eigen::Triplet<double>>& t, const Eigen::MatrixXd& a, const Element& FE) const; // Ассамблирование матрицы A размерности [DOF x DOF] из меньшей матрицы a

    public:
        /*Конструктор класса*/
        TFE_model(const size_t dx, const size_t dy, const size_t dz); // Инициализация сетки с заданным количеством элементов по каждому направлению

        /*Добавление структурных единиц*/
        void add_node(const Point p, const int g_nbr);
        void add_node(const Node& node);
        void add_element(const ElementType fe_type, const std::vector<const Node*>& verts, const int& g_nbr, const Material* const material, const bool is_surf = false, const float& surf_area = 0, std::string* const layer = nullptr, std::string* const primitive = nullptr);

        /*Предрасчёт сетки*/
        void pre_calculate();
        void mesh_check();
        void surface_check();

        /*Вычисление параметров*/
        Eigen::SparseMatrix<double> GCM(const Eigen::VectorXd& nodal_temps) const; // Глобальная матрица теплопроводности
        Eigen::SparseMatrix<double> GDM(const Eigen::VectorXd& nodal_temps) const; // Глобальная матрица демпфирования
        Eigen::SparseVector<double> NLV(const double q, const double eps, const Eigen::VectorXd& nodal_temps) const; // Вектор узловых нагрузок

        /*Решение нестационарной задачи с заданными начальными условиями, временем расчёта и шагом.*/
        Results_transient transient_analisys(const std::vector<std::pair<int, double>>& constraints, const float q) const; 
        Eigen::VectorXd steady_state_analysis(const std::vector<std::pair<int, double>>& constraints, const float q) const;
        /*Вывод объектов*/
        const std::vector<Node>& Nodes() const;
        const std::vector<Element>& Elements() const;

        /*Вывод информации*/
        void mesh_info() const;
};
