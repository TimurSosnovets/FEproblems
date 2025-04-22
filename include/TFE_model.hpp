#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
// Current project
#include "FE_entities.hpp"
#include "Materials.hpp"
#include "LQube_IP.hpp"
#include "LWedge_IP.hpp"
#include "Output.hpp"
#include "Ballistic_data.hpp"
// Eigen
#include <IterativeLinearSolvers>
#include <SparseCore>
#include <OrderingMethods>
#include <PardisoSupport>
// STL
#include <unordered_set>
#include <chrono>
#include <algorithm>
#include <iomanip> // for std::setprecision
#include <omp.h>
// VTK includes
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>
#include <vtkXMLUnstructuredGridWriter.h>
#include <vtkPoints.h>
#include <vtkWedge.h>
#include <vtkHexahedron.h>
#include <vtkTriangle.h>
#include <vtkQuad.h>
#include <vtkStringArray.h>
#include <vtkDataSetMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkProperty.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>
#include <vtkFieldData.h>
#include <vtkPointData.h>
#include <vtkZLibDataCompressor.h>

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
        size_t _DOF = 0; // Степень свободы модели (в данном случае оно же - количество узлов)
        size_t unique_DOF = 0; // Количество ненулевых значений в матрицах (зависит только от сетки)
        size_t unique_DOF_surf = 0; // Количество ненулевых значений в векторе нагрузок (зависит только от сетки)
        const Geometry& geometry;
        const Layers& layers;

        /*Внутренние методы*/
        void assembly(std::vector<Eigen::Triplet<double>>& t, const Eigen::MatrixXd& a, const Element& FE) const; // Ассамблирование матрицы A размерности [DOF x DOF] из меньшей матрицы a

    public:
        /*Конструктор класса*/
        TFE_model(Geometry& g, Layers& l); // Инициализация сетки с заданным количеством элементов по каждому направлению

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
        Eigen::SparseVector<double> Ball_NLV(const double eps, const double vel, const double dens, const double Kn, const Eigen::VectorXd& nodal_temps) const; // Вектор узловых нагрузок

        /*Решение нестационарной задачи с заданными начальными условиями, временем расчёта и шагом.*/
        Results_transient transient_analisys(const std::vector<std::pair<int, double>>& constraints, const float q) const;
        std::vector<std::pair<double, Eigen::VectorXd>> transient_analisys() const; 
        Eigen::VectorXd steady_state_analysis(const std::vector<std::pair<int, double>>& constraints, const float q, const bool radiation = false) const;
        /*Для отладки*/
        Eigen::VectorXd jacobian_check() const;
        Eigen::VectorXd get_surface_load(double t) const;
        /*Вывод объектов*/
        const std::vector<Node>& Nodes() const;
        const std::vector<Element>& Elements() const;

        /*Вывод информации*/
        void mesh_info() const;
        void export_to_vtk(const std::string& filename, bool visualize = true) const;
        void export_to_vtk(const std::string& filename, const std::vector<std::pair<double, Eigen::VectorXd>>& transient_results) const;
        void create_mesh_file(const std::string& filename, const std::vector<std::pair<double, Eigen::VectorXd>>& transient_results) const;
        void create_static_mesh_file(const std::string& filename, const Eigen::VectorXd& jacobians) const;
        void create_surface_mesh_file(const std::string& filename_prefix, const std::vector<std::pair<double, Eigen::VectorXd>>& elemental_load) const;

};

// Заполнение FE модели
void make_model(TFE_model& model, Layers& layer, Geometry& geom, int c_phi);
void make_model_advance(TFE_model& model, Layers& layer, Geometry& geom, int c_phi);

