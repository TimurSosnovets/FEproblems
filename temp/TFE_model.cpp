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
void TFE_model::add_element(const ElementType fe_type, const std::vector<Node*>& verts, const int& g_nbr, const Material* const material, const bool is_surf = false, const float& surf_area = 0, std::string* const layer = nullptr, std::string* const primitive = nullptr)
{
    std::unique_ptr<Isoparametric_3D> type;
    if (fe_type == ElementType::LQube) {type = std::make_unique<LQube>();}
    auto& new_element = _elements.emplace_back(std::move(type), verts, g_nbr, material, is_surf, surf_area);
    if (layer) {new_element.set_layer(layer);}
    if (primitive) {new_element.set_primitive(primitive);}

}

// Предрасчёт элемента
void TFE_model::pre_calculate()
{
    for (auto& element : _elements)
    {
        element.type->calculate_element(element);
    }
}

// Ассамблирование матрицы A размерности [DOF x DOF] из меньшей матрицы a
void TFE_model::assembly(std::vector<Eigen::Triplet<double>>& t , const Eigen::MatrixXd& a, const Element& FE) const
{
    for (size_t i = 0; i < a.rows(); ++i)
    {
        for (size_t j = 0; j < a.cols(); ++j)
        {
            size_t row = FE.vertices[i]->gn - 1;
            size_t col = FE.vertices[j]->gn - 1;
            t.emplace_back(row, col, a(i, j));
        }
    }
} 

// Проверка количества ненулевых значений в глобальных матрицах
void TFE_model::mesh_check()
{
    std::unordered_set<std::pair<int, int>, PairHash> nnz_entries;

    for (const auto& element : _elements)
    {
        for (int i = 0; i < element.vertices.size(); ++i) 
        {
            for (int j = 0; j < element.vertices.size(); ++j) 
            {
                int row = element.vertices[i]->gn - 1;
                int col = element.vertices[j]->gn - 1;
                nnz_entries.emplace(row, col);
            }
        }
    }
    unique_DOF = nnz_entries.size();
}

// Глобальная матрица теплопроводности
Eigen::SparseMatrix<double> TFE_model::GCM(const Eigen::VectorXd& nodal_temps) const
{
    /*Инициализация*/
    Eigen::SparseMatrix<double> GCM;
    std::vector<Eigen::Triplet<double>> triplets;
    if (!(unique_DOF == 0)) {triplets.reserve(unique_DOF);}
    Eigen::Matrix<double, 8, 8> H;
    Eigen::Vector<double, 8> T;

    /*Заполнение вектора ненулевых значений*/
    for (const auto& element : _elements)
    {   
        // Заполняем локальный вектор узловых температур
        int i = 0;
        for (const auto& node : element.vertices)
        {
            T[i] = nodal_temps[node->gn - 1];
            ++i;
        }

        H = element.type->Cond_Mat(element, T);
        assembly(triplets, H, element);
    }

    GCM.setFromTriplets(triplets.begin(), triplets.end());
    return GCM;
} 

// Глобальная матрица демфирования
Eigen::SparseMatrix<double> TFE_model::GDM(const Eigen::VectorXd& nodal_temps) const
{
    /*Инициализация*/
    Eigen::SparseMatrix<double> GDM;
    std::vector<Eigen::Triplet<double>> triplets;
    if (!(unique_DOF == 0)) {triplets.reserve(unique_DOF);}
    Eigen::Matrix<double, 8, 8> C;
    Eigen::Vector<double, 8> T;

    /*Заполнение вектора ненулевых значений*/
    for (const auto& element : _elements)
    {   
        // Вектор узловых температур
        int i = 0;
        for (const auto& node : element.vertices)
        {
            T[i] = nodal_temps[node->gn - 1];
            ++i;
        }

        C = element.type->Damp_Mat(element, T);
        assembly(triplets, C, element);
    }

    GDM.setFromTriplets(triplets.begin(), triplets.end());
    return GDM;
} 

// Вектор узловых нагрузок
Eigen::SparseVector<double> TFE_model::NLV(const double q, const double eps, const Eigen::VectorXd& nodal_temps) const
{
    /*Инициализация*/
    Eigen::SparseVector<double> NLV;
    std::vector<Eigen::Triplet<double>> triplets;

    size_t surf_el_count = 0; // Количество поверхностных элементов
    for (const auto& element : _elements)
    {
        if (element.is_surface) ++surf_el_count;
    }
    triplets.reserve(surf_el_count * 2);

    Eigen::Vector<double, 8> F;
    Eigen::Vector<double, 8> T;

    /*Заполнение вектора ненулевых значений*/
    for (const auto& element : _elements)
    {   
        // Вектор узловых температур
        int i = 0;
        for (const auto& node : element.vertices)
        {
            T[i] = nodal_temps[node->gn - 1];
            ++i;
        }

        F = element.type->Heat_Load_Surf(element, q, eps, T);
        //
    }


}




