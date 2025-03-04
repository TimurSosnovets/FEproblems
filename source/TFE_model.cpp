//Current project
#include "TFE_model.hpp"
#include "Output.hpp"

// Конструктор класса
TFE_model::TFE_model(const size_t dx, const size_t dy, const size_t dz) : _DOF((dx+1) * (dy+1) * (dz+1))
{
    _nodes.reserve(_DOF);
    _elements.reserve(dx * dy * dz);
}

// Вывод объектов
const std::vector<Node>& TFE_model::Nodes() const {return _nodes;}
const std::vector<Element>& TFE_model::Elements() const {return _elements;}

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
    message = "\n============\nElement info\n============\n";
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
void TFE_model::add_node(const Node& node)
{
    _nodes.emplace_back(node);
}

// Добавление элемента
void TFE_model::add_element(const ElementType fe_type, const std::vector<const Node*>& verts, const int& g_nbr, const Material* const material, const bool is_surf, const float& surf_area, std::string* const layer, std::string* const primitive)
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

// Сбор триплетов для разреженной матрицы размерности [DOF x DOF] из меньшей матрицы a
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
    std::cout << "Unique DOF count: " << unique_DOF << std::endl;
}

void TFE_model::surface_check()
{
    std::unordered_set<int> nnz_nodes;

    for (const auto& element : _elements)
    {
        if (element.is_surface)
        {
            for (const auto& vertex : element.vertices)
            {
                nnz_nodes.insert(vertex->gn - 1);
            }
        }
    }
    unique_DOF_surf = nnz_nodes.size();
}

// Глобальная матрица теплопроводности
Eigen::SparseMatrix<double> TFE_model::GCM(const Eigen::VectorXd& nodal_temps) const
{
    /*Инициализация*/
    Eigen::SparseMatrix<double> GCM(_DOF, _DOF);
    std::vector<Eigen::Triplet<double>> triplets;
    // Размерность
    size_t size = 8;
    triplets.reserve(_elements.size() * size * size);
    Eigen::MatrixXd H(size, size);
    Eigen::VectorXd T(size);


    /*Заполнение вектора ненулевых значений*/
    for (const auto& element : _elements)
    {   
        size = element.vertices.size();
        // Заполняем локальный вектор узловых температур
        for (size_t i = 0; i < size; ++i)
        {
            T[i] = nodal_temps[element.vertices[i]->gn - 1];
        }

        H.topLeftCorner(size, size) = element.type->Cond_Mat(element, T.head(size));
        assembly(triplets, H.topLeftCorner(size, size), element);
    }

    GCM.setFromTriplets(triplets.begin(), triplets.end());
    return GCM;
} 

// Глобальная матрица демфирования
Eigen::SparseMatrix<double> TFE_model::GDM(const Eigen::VectorXd& nodal_temps) const
{
    /*Инициализация*/
    Eigen::SparseMatrix<double> GDM(_DOF, _DOF);
    std::vector<Eigen::Triplet<double>> triplets;
    // Размерность
    size_t size = 8;
    triplets.reserve(_elements.size() * size * size);
    Eigen::MatrixXd C(size, size);
    Eigen::VectorXd T(size);

    /*Заполнение вектора ненулевых значений*/
    for (const auto& element : _elements)
    {   
        size = element.vertices.size();
        // Вектор узловых температур
        for (size_t i = 0; i < size; ++i)
        {
            T[i] = nodal_temps[element.vertices[i]->gn - 1];
        }

        C.topLeftCorner(size, size) = element.type->Damp_Mat(element, T.head(size));
        assembly(triplets, C.topLeftCorner(size, size), element);
    }

    GDM.setFromTriplets(triplets.begin(), triplets.end());
    return GDM;
} 

// Вектор узловых нагрузок
Eigen::SparseVector<double> TFE_model::NLV(const double q, const double eps, const Eigen::VectorXd& nodal_temps) const
{
    /*Инициализация*/
    Eigen::SparseVector<double> NLV(_DOF);
    // Размерность векторов
    size_t size = 8;
    Eigen::VectorXd F(size);
    Eigen::VectorXd T(size);

    /*Заполнение вектора ненулевых значений*/
    for (const auto& element : _elements)
    {   
        int i = 0;
        for (const auto& node : element.vertices)
        {
            T[i] = nodal_temps[node->gn - 1];
            ++i;
        }

        F = element.type->Heat_Load_Surf(element, q, eps, T.head(element.vertices.size()));
        
        // Перенос значений в глобальный вектор
        for (size_t i = 0; i < element.vertices.size(); ++i)
        {
            NLV.coeffRef(element.vertices[i]->gn - 1) += F[i];
        }
    }

    return NLV;
}

// Динамический расчет 
Results_transient TFE_model::transient_analisys(const std::vector<std::pair<int, double>>& constraints, const float q) const
{
    /*Инициализация ввода данных*/
    float max_time, time_step, time_step_output, initial_temp;
    std::vector<size_t> node_samples;
    size_t node_sample;
    std::vector<float> time_samples;
    float time_sample;
    float eps = 1e-6;

    /*Непосредственный ввод данных*/
    logger::log("Enter initial temperature (K):");
    std::cin >> initial_temp;

    logger::log("Enter the calculation time interval (s):");
    std::cin >> max_time;

    logger::log("Enter time step (s):");
    std::cin >> time_step;

    logger::log("Enter numbers of nodes (1 based) you want to check through time (-1 to finish input)");
    while (true) {
        std::cin >> node_sample;

        // Проверка адекватности введённого значения по типу
        if (std::cin.fail()) 
        {
            std::cin.clear(); 
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
            logger::log("Invalid input. Please enter an integer.");
            continue; 
        }

        // Проверка адекватности введённого значения по диапазону
        if (node_sample != -1 && (node_sample < 1 || node_sample > _nodes.size() + 1)) 
        {
            std::cin.setstate(std::ios::failbit); // Force cin to fail
            logger::log("Value out of range. Should be above 0 and below " + std::to_string(_nodes.size() + 1) + ".");
            continue;
        }

        if (node_sample == -1) {break;}

        node_samples.push_back(node_sample);
    }

    logger::log("Enter time step for outputed values (s) (better be a multiple of actual time step for computation):");
    std::cin >> time_step_output;

    logger::log("Enter specific time moments (s) in which you want nodal temperatures to be outputed (-1 to finish input):");
    while (true) {
        std::cin >> time_sample;

        // Проверка адекватности введённого значения по типу
        if (std::cin.fail()) 
        {
            std::cin.clear(); 
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
            logger::log("Invalid input. Please enter an integer or float value.");
            continue; 
        }

        // Проверка адекватности введённого значения по диапазону
        if (time_sample != -1 && (time_sample <= 0 || time_sample > max_time)) 
        {
            std::cin.setstate(std::ios::failbit); // Force cin to fail
            logger::log("Value out of range. Please enter value above zero.");
            continue;
        }

        if (time_sample == -1) {break;}

        time_samples.push_back(time_sample);
    }
    
    /*Инициализация расчёта*/
    auto start = std::chrono::high_resolution_clock::now(); // Таймер
    std::string message;
    Eigen::VectorXd nodal_temps = initial_temp * Eigen::VectorXd::Ones(_DOF); // Глобальный вектор узловых температур
    Eigen::SparseMatrix<double> Lh(_DOF, _DOF); // Матрица левой части матричного уравнения
    Eigen::VectorXd Rh(_DOF); // Вектор правой части матричного уравнения
    Eigen::BiCGSTAB<Eigen::SparseMatrix<double>> solver; // Решатель
    // Заготовка под вывод значений
    Results_transient results(time_samples, node_samples, _DOF, static_cast<size_t>(std::ceil(max_time / time_step_output)));
    //Results_transient results(time_samples, node_samples, _DOF, 10);
    logger::log("Started transient analysis calculation.");
    /*Расчёт*/
    for (size_t t = 0; t * time_step < max_time; ++t)
    {
        // Вычисление значений на шаге
        Lh = GCM(nodal_temps) + (2 / time_step) * (GDM(nodal_temps));
        Rh = ((2 / time_step) * GDM(nodal_temps) - GCM(nodal_temps)) * nodal_temps + 2 * NLV(q, 0.0, nodal_temps);
        // Закрепления
        if (!constraints.empty())
        {
            for (const auto& LBC : constraints)
            {
                Rh(LBC.first) = LBC.second;
                //for (Eigen::SparseMatrix<double>::InnerIterator it(Lh, LBC.first); it; ++it) {it.valueRef() = 0;}
                for (int col = 0; col < Lh.cols(); ++col) {
                    Lh.coeffRef(LBC.first, col) = 0; // Explicitly set all elements in the row to 0
                }
                Lh.coeffRef(LBC.first, LBC.first) = 1;
            }
        }
        Lh.prune(0.0);
        // std::cout << "Left hand matrix:\n" << Lh.toDense() << std::endl;
        // std::cin.get();
        // std::cout << "Right hand vector:\n" << Rh << std::endl;
        // std::cin.get();
        // std::cin.get();
        // Решение матричного уравнения
        solver.compute(Lh);
        if (solver.info() != Eigen::Success) {std::cerr << "Solver setup failed!\n";}
        nodal_temps = solver.solve(Rh);
        if (solver.info() != Eigen::Success) {std::cerr << "Solving failed!\n";}
        else {std::cout << "Time step " << t << " done." << std::endl;}
        
        // // Запись значений
        // for (int i = 0; i < time_samples.size(); ++i)
        // {
        //     if (abs(time_samples[i] - t * time_step) < time_step) {results.VNT_samples[i].second = nodal_temps;}
        // }

        // if (std::remainder((t * time_step), time_step_output) < eps)
        if (true)
        {
            results.NT_samples[0].second[0] = 0.0;
            for (int i = 0; i < node_samples.size(); ++i)
            {
                results.NT_samples[i].second.emplace_back(nodal_temps(node_samples[i] - 1));
                ++i;
            }
        }
    }    

    /*Вывод времени расчёта*/
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    message = "Execution time: " + std::to_string(elapsed_seconds.count()) + " seconds.";
    logger::log(message);
    std::cin.get();

    return results;
}

Eigen::VectorXd TFE_model::steady_state_analysis(const std::vector<std::pair<int, double>>& constraints, const float q) const
{
    /*Инициализация*/
    auto start = std::chrono::high_resolution_clock::now(); // Таймер
    std::string message;
    Eigen::VectorXd nodal_temps; // Глобальный вектор узловых температур
    Eigen::SparseMatrix<double> Lh(_DOF, _DOF); // Матрица левой части матричного уравнения
    Eigen::VectorXd Rh(_DOF); // Вектор правой части матричного уравнения
    Eigen::SparseQR<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>> solver;
    logger::log("Started transient analysis calculation.");

    Lh = GCM(300 * Eigen::VectorXd::Ones(_DOF));
    Rh = NLV(q, 0.0, 300 * Eigen::VectorXd::Ones(_DOF));

    // Закрепления
    if (!constraints.empty())
    {
        for (const auto& LBC : constraints)
        {
            Rh(LBC.first) = LBC.second;
            for (int col = 0; col < Lh.cols(); ++col) {
                Lh.coeffRef(LBC.first, col) = 0; // Explicitly set all elements in the row to 0
            }
            Lh.coeffRef(LBC.first, LBC.first) = 1;
        }
    }
    Lh.prune(0.0);

    std::cout << "Left hand matrix:\n" << Lh.toDense() << std::endl;
    std::cout << "Right hand vector:\n" << Rh << std::endl;

    solver.compute(Lh);
    if (solver.info() != Eigen::Success) {std::cerr << "Solver setup failed!\n";}
    nodal_temps = solver.solve(Rh);
    if (solver.info() != Eigen::Success) {std::cerr << "Solving failed!\n";}

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    message = "Execution time: " + std::to_string(elapsed_seconds.count()) + " seconds.";
    logger::log(message);
    std::cin.get();
    return nodal_temps;
}