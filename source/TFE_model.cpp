//Current project
#include "TFE_model.hpp"
#include "Output.hpp"

// Конструктор класса
TFE_model::TFE_model(Geometry& g, Layers& l) : geometry(g), layers(l) {}

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
    logger::log(message, true, filename);

    /*Вывод информации по элементам*/
    message = "============\nElement info\n============\n";
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
    if (fe_type == ElementType::LWedge) {type = std::make_unique<LWedge>();}
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
    _DOF = _nodes.size();
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
    if (_DOF == 0) {throw std::invalid_argument("Mesh check is required!");}
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
    if (_DOF == 0) {throw std::invalid_argument("Mesh check is required!");}
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
    if (_DOF == 0) {throw std::invalid_argument("Mesh check is required!");}
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

// Вектор узловых нагрузок
Eigen::SparseVector<double> TFE_model::Ball_NLV(const double eps, const double vel, const double dens, const double Kn, const Eigen::VectorXd& nodal_temps) const
{
    if (_DOF == 0) {throw std::invalid_argument("Mesh check is required!");}
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

        F = element.type->Ball_heat_load(element, geometry, eps, vel, dens, Kn, T.head(element.vertices.size()));
        
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

        if (time_sample == -1) {break;}

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
            logger::log("Value out of range. Please enter value above zero and below " + std::to_string(max_time) + " seconds.");
            continue;
        }

        time_samples.push_back(time_sample);
    }
    // Сортировка значений времени по возрастанию
    std::sort(time_samples.begin(), time_samples.end(), [](float a, float b) {return abs(a) < abs(b);});
    float* t_ptr = time_samples.data();
    float* last_t = t_ptr + time_samples.size() - 1;

    /*Инициализация расчёта*/
    auto start = std::chrono::high_resolution_clock::now(); // Таймер
    std::string message;
    Eigen::VectorXd nodal_temps = initial_temp * Eigen::VectorXd::Ones(_DOF); // Глобальный вектор узловых температур
    Eigen::SparseMatrix<double> Lh(_DOF, _DOF); // Матрица левой части матричного уравнения
    Eigen::VectorXd Rh(_DOF); // Вектор правой части матричного уравнения
    Eigen::BiCGSTAB<Eigen::SparseMatrix<double>> solver; // Решатель
    // Заготовка под вывод значений
    Results_transient results(time_samples, node_samples, _DOF, static_cast<size_t>(std::ceil(max_time / time_step_output)));
 
    /*Расчёт*/
    logger::log("Started transient analysis calculation.");
    for (size_t t = 1; (t-1) * time_step < max_time; ++t)
    {
        // Вычисление значений на шаге
        Lh = GCM(nodal_temps) + (2 / time_step) * (GDM(nodal_temps));
        Rh = ((2 / time_step) * GDM(nodal_temps) - GCM(nodal_temps)) * nodal_temps - 2 * NLV(q, 0.0, nodal_temps);

        // Закрепление системы
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
        
        // Решение матричного уравнения
        solver.compute(Lh);
        if (solver.info() != Eigen::Success) {std::cerr << "Solver setup failed!\n";}
        nodal_temps = solver.solve(Rh);
        if (solver.info() != Eigen::Success) {std::cerr << "Solving failed!\n";}
        else 
        {
            std::cout << "\rProgress: " << std::fixed << std::setprecision(2)
              << (100.0 * t * time_step / max_time) << "% " << std::flush;
        }
        
        // Запись значений
        if (abs(*t_ptr - t * time_step) < eps)
        {
            results.VNT_samples.emplace_back(std::make_pair(*t_ptr, nodal_temps));
            if (t_ptr != last_t) {++t_ptr;}
        }

        if (t % static_cast<int>(time_step_output / time_step) < eps)
        {
            for (int i = 0; i < node_samples.size(); ++i)
            {
                results.NT_samples[i].second.emplace_back(nodal_temps(node_samples[i] - 1));
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

Eigen::VectorXd TFE_model::steady_state_analysis(const std::vector<std::pair<int, double>>& constraints, const float q, const bool radiation) const
{
    /*Инициализация*/
    auto start = std::chrono::high_resolution_clock::now(); // Таймер
    std::string message;
    Eigen::VectorXd nodal_temps; // Глобальный вектор узловых температур
    Eigen::SparseMatrix<double> Lh(_DOF, _DOF); // Матрица левой части матричного уравнения
    Eigen::VectorXd Rh(_DOF); // Вектор правой части матричного уравнения
    Eigen::SparseQR<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>> solver;
    // Eigen::BiCGSTAB<Eigen::SparseMatrix<double>> solver;
    // solver.setMaxIterations(100);
    // solver.setTolerance(1e-3);
    logger::log("Started steady state analysis calculation.");
    double eps = 0;
    if (radiation) {eps = 0.9;}
    Lh = GCM(300 * Eigen::VectorXd::Ones(_DOF));
    logger::log("Lh was setted!");
    Rh = NLV(q, eps, 300 * Eigen::VectorXd::Ones(_DOF));
    logger::log("Rh was setted!");

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

    // std::cout << "Left hand matrix:\n" << Lh.toDense() << std::endl;
    // std::cout << "Right hand vector:\n" << Rh << std::endl;

    // Step 1: Compute permutation to reduce bandwidth
    Eigen::AMDOrdering<int> ordering;
    Eigen::PermutationMatrix<Eigen::Dynamic, Eigen::Dynamic> perm;
    ordering(Lh, perm);  // Compute permutation

    // Step 2: Apply reordering to matrix and RHS
    Lh = perm * Lh * perm.transpose();
    Rh = perm * Rh;

    Lh.makeCompressed();
    solver.compute(Lh);
    if (solver.info() != Eigen::Success) {std::cerr << "Solver setup failed!\n";}
    nodal_temps = solver.solve(Rh);
    if (solver.info() != Eigen::Success) {std::cerr << "Solving failed!\n";}

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    message = "Execution time: " + std::to_string(elapsed_seconds.count()) + " seconds.";
    logger::log(message);
    std::cin.get();

    return perm.transpose() * nodal_temps;
    // return nodal_temps;
}

void TFE_model::transient_analisys() const
{
    /*Инициализация ввода данных*/
    float max_time, time_step, time_step_output, initial_temp;
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

    logger::log("Enter time step for outputed values (s) (better be a multiple of actual time step for computation):");
    std::cin >> time_step_output;

    // logger::log("Enter specific time moments (s) in which you want nodal temperatures to be outputed (-1 to finish input):");
    // while (true) {
    //     std::cin >> time_sample;

    //     if (time_sample == -1) {break;}

    //     // Проверка адекватности введённого значения по типу
    //     if (std::cin.fail()) 
    //     {
    //         std::cin.clear(); 
    //         std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
    //         logger::log("Invalid input. Please enter an integer or float value.");
    //         continue; 
    //     }

    //     // Проверка адекватности введённого значения по диапазону
    //     if (time_sample != -1 && (time_sample <= 0 || time_sample > max_time)) 
    //     {
    //         std::cin.setstate(std::ios::failbit); // Force cin to fail
    //         logger::log("Value out of range. Please enter value above zero and below " + std::to_string(max_time) + " seconds.");
    //         continue;
    //     }

    //     time_samples.push_back(time_sample);
    // }
    // Сортировка значений времени по возрастанию
    std::sort(time_samples.begin(), time_samples.end(), [](float a, float b) {return abs(a) < abs(b);});
    float* t_ptr = time_samples.data();
    float* last_t = t_ptr + time_samples.size() - 1;

    /*Инициализация расчёта*/
    auto start = std::chrono::high_resolution_clock::now(); // Таймер
    std::string message;
    Eigen::VectorXd nodal_temps = initial_temp * Eigen::VectorXd::Ones(_DOF); // Глобальный вектор узловых температур
    Eigen::SparseMatrix<double> Lh(_DOF, _DOF); // Матрица левой части матричного уравнения
    Eigen::VectorXd Rh(_DOF); // Вектор правой части матричного уравнения
    std::vector<std::pair<std::string, Eigen::VectorXd>> results;

    /*Решатель и его настройки*/
    Eigen::ConjugateGradient<Eigen::SparseMatrix<double>> solver; 
    solver.setMaxIterations(100);
    solver.setTolerance(1e-6);

    /*Расчёт*/
    logger::log("Started transient analysis calculation.");
    Ballistic_data data("Ballistics_CD.csv");
    double vel, dens, Kn, eps_grey = 0.9;
    for (size_t t = 1; (t-1) * time_step < max_time; ++t)
    {
        // Вычисление значений на шаге
        vel = data.get_Velocity(t * time_step);
        dens = data.get_Density(t * time_step);
        Kn = data.get_Knudsen(t * time_step);
        Lh = GCM(nodal_temps) + (2 / time_step) * (GDM(nodal_temps));
        Rh = ((2 / time_step) * GDM(nodal_temps) - GCM(nodal_temps)) * nodal_temps + 2 * Ball_NLV(eps_grey, vel, dens, Kn, nodal_temps);
        
        // Решение матричного уравнения
        Lh.makeCompressed();
        
        solver.compute(Lh);
        if (solver.info() != Eigen::Success) {std::cerr << "Solver setup failed!\n";}
        nodal_temps = solver.solve(Rh);
        if (solver.info() != Eigen::Success) {std::cerr << "Solving failed!\n";}
        else 
        {
            std::cout << "\rProgress: " << std::fixed << std::setprecision(2)
              << (100.0 * t * time_step / max_time) << "% " << std::flush;
        }
        
        // Запись значений
        // if (abs(*t_ptr - t * time_step) < eps)
        // {
        //     std::string header = "time " + std::to_string(t * time_step) + " s";
        //     results.emplace_back(std::make_pair(header, nodal_temps));
        //     logger::log("Temperatures for " + header + " were written!");
        //     if (t_ptr != last_t) {++t_ptr;}
        // }
        if (t % static_cast<int>(time_step_output / time_step) < eps)
        {
            std::string header = "time " + std::to_string(t * time_step) + " s";
            results.emplace_back(std::make_pair(header, nodal_temps));
        }
    }    
    Save_xlsx(results);
    /*Вывод времени расчёта*/
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    message = "Execution time: " + std::to_string(elapsed_seconds.count()) + " seconds.";
    logger::log(message);
    std::cin.get();
}

// Заполнение КЭ модели
void make_model(TFE_model& model, Layers& layer, Geometry& geom, int c_phi)
{
    /*Геометрия*/
    double R_sph = 0.336;
    double x_sph = R_sph * (1 - cos(deg2rad(69.0)));

    /*Заполнение массива узлов*/
    size_t nbr = 1;
    double x, y, z;
    double r, h, phi;
    double d_x, d_phi = 2 * M_PI / c_phi;

    // СФЕРА - первые узлы
    for (int it_h = 0; it_h <= layer.FRNT; ++it_h)
    {
        y = 0;
        z = 0;
        x = layer.depth(it_h);
        model.add_node(Point(x, y, z), nbr);
        // Выборка узлов
        
        ++nbr;
    }

    // СФЕРА - основные узлы
    for (int it_x = 1; it_x <= geom.FE_sph; ++it_x)
    {
        for (int it_phi = 0; it_phi < c_phi; ++it_phi)
        {
            for (int it_h = 0; it_h <= layer.FRNT; ++it_h)
            {
                h = layer.depth(it_h);
                d_x = (geom.x_refers[0] - h) / geom.FE_sph;
                x = h + it_x * d_x;
                r = r_circle(x, h, geom.R_sphere);
                phi = it_phi * d_phi;
                y = (-1) * r * cos(phi);
                z = r * sin(phi);

                model.add_node(Point(x, y, z), nbr);
                ++nbr;
            }
        }
    }
    
    // КОНУС 1
    Point Sph_Cn1(geom.x_refers[0], -geom.r_refers[0]), Cn1_Cn2(geom.x_refers[1], -geom.r_refers[1]); // Точки перехода поверхностей внешних обводов СА
    for (int it_x = 1; it_x <= geom.FE_cone1; ++it_x)
    {
        for (int it_phi = 0; it_phi < c_phi; ++it_phi)
        {
            for (int it_h = 0; it_h <= layer.FRNT; ++it_h)
            {
                h = layer.depth(it_h);
                d_x = (geom.x_refers[1]- geom.x_refers[0]) / geom.FE_cone1;
                x = geom.x_refers[0] + it_x * d_x;
                r = - r_line(x, h, Sph_Cn1, Cn1_Cn2);
                phi = it_phi * d_phi;
                y = (-1) * r * cos(phi);
                z = r * sin(phi);

                model.add_node(Point(x, y, z), nbr);
                ++nbr;
            }
        }
    }

    // КОНУС 2
    Point Cn2_Cyl(geom.x_refers[2], -geom.r_refers[2]); // Точки перехода поверхностей внешних обводов СА
    for (int it_x = 1; it_x <= geom.FE_cone2; ++it_x)
    {
        for (int it_phi = 0; it_phi < c_phi; ++it_phi)
        {
            for (int it_h = 0; it_h <= layer.FRNT; ++it_h)
            {
                h = layer.depth(it_h);
                d_x = (geom.x_refers[2]- geom.x_refers[1]) / geom.FE_cone2;
                x = geom.x_refers[1] + it_x * d_x;
                r = - r_line(x, h, Cn1_Cn2, Cn2_Cyl);
                phi = it_phi * d_phi;
                y = (-1) * r * cos(phi);
                z = r * sin(phi);

                model.add_node(Point(x, y, z), nbr);
                ++nbr;
            }
        }
    }

    // Цилиндр
    Point Cyl_Bot(geom.x_refers[3], -geom.r_refers[3]); // Точки перехода поверхностей внешних обводов СА
    for (int it_x = 1; it_x <= geom.FE_cyl; ++it_x)
    {
        for (int it_phi = 0; it_phi < c_phi; ++it_phi)
        {
            for (int it_h = 0; it_h <= layer.FRNT; ++it_h)
            {
                h = layer.depth(it_h);
                d_x = (geom.x_refers[3]- geom.x_refers[2] - h) / geom.FE_cyl;
                x = geom.x_refers[2] + it_x * d_x;
                r = - r_line(x, h, Cn2_Cyl, Cyl_Bot);
                phi = it_phi * d_phi;
                y = (-1) * r * cos(phi);
                z = r * sin(phi);

                model.add_node(Point(x, y, z), nbr);
                ++nbr;
            }
        }
    }

    // ДНИЩЕ - Последние узлы
    for (int it_h = 0; it_h <= layer.FRNT; ++it_h)
    {
        y = 0;
        z = 0;
        x = geom.x_refers[3] - layer.depth(it_h);
        model.add_node(Point(x, y, z), nbr);
        ++nbr;
    }

    // Номера соседних узлов
    auto next_x = [layer, c_phi] (int node) -> int 
    {
        if (node <= layer.FRNT + 1) {return  node + layer.FRNT + 1;}
        else {return node + (layer.FRNT + 1) * c_phi;}
    };
    auto next_h = [layer, c_phi] (int node) -> int {return node + 1;};
    auto next_phi = [layer, c_phi] (int node) -> int 
    {
        if (node <= layer.FRNT + 1) {return node;}

        int rem = (node - (layer.FRNT + 1)) % ((layer.FRNT + 1) * (c_phi));
        if ((rem > (c_phi - 1) * (layer.FRNT + 1)) || (rem == 0)) {return node - (layer.FRNT + 1) * (c_phi - 1);}

        return node + layer.FRNT + 1;
    };
    auto bot_iter = [layer, c_phi] (int node) -> int {return node - (layer.FRNT + 1);};
    
    // // Вывод информации об узлах
    // for (const auto& node : model.Nodes())
    // {
    //     std::cout << std::fixed << std::setprecision(2) 
    //     << "Node: " << node.global_number() 
    //     << " X = " << node.coords().x * 1000 << ", Y = " << node.coords().y * 1000 << ", Z = " << node.coords().z * 1000<< ".\n"; 
    //     std::cout << "next_x " << next_x(node.global_number()) << std::endl;
    //     std::cout << "next_h " << next_h(node.global_number()) << std::endl;
    //     std::cout << "next_phi " << next_phi(node.global_number()) << std::endl << std::endl;
    // }
    
    /*Заполнение массива элементов*/
    std::vector<const Node*> vts;
    nbr = 1;
    int base, v1 = 1, v2, v3, v4, v5, v6, v7, v8;

    // Носик
    vts.resize(6);
    base = 1;
    for (int it_h = 0; it_h < layer.FRNT; ++it_h)
    {
        v1 = base;
        v2 = next_x(v1);
        for (int it_p = 0; it_p < c_phi; ++it_p)
        {
            // Формируем массив вершин
            v3 = next_phi(v2);
            v4 = next_h(v1);
            v5 = next_h(v2);
            v6 = next_h(v3);
            vts[0] = &model.Nodes()[v1 - 1];
            vts[1] = &model.Nodes()[v3 - 1];
            vts[2] = &model.Nodes()[v2 - 1];
            vts[3] = &model.Nodes()[v4 - 1];
            vts[4] = &model.Nodes()[v6 - 1];
            vts[5] = &model.Nodes()[v5 - 1];

            // Характеристики элемента
            bool is_surface = true;
            if (it_h > 0) {is_surface = false;}
            const Material* material = layer.get_material(it_h);
            // const Material* material = &AMg_6;
            std::string* layer_name = layer.get_name(it_h);
            std::string* primitive_name = geom.get_name(0);
            
            // Создание элемента
            model.add_element(ElementType::LWedge, vts, nbr, material, is_surface, 1.0, layer_name, primitive_name);
            ++nbr;

            v2 = next_phi(v2);
        }
        base = next_h(base);
    }

    // Основная часть
    vts.resize(8);
    base = next_x(1);
    for (int it_x = 1; it_x < geom.FE_all; ++it_x)
    {
        v1 = base;
        for (int it_h = 0; it_h < layer.FRNT; ++it_h)
        {
            for (int it_p = 0; it_p < c_phi; ++it_p)
            {
                // Формируем массив вершин
                v2 = next_phi(v1);
                v3 = next_x(v2);
                v4 = next_x(v1);
                v5 = next_h(v1);
                v6 = next_h(v2);
                v7 = next_h(v3);
                v8 = next_h(v4);
                vts[0] = &model.Nodes()[v1 - 1];
                vts[1] = &model.Nodes()[v2 - 1];
                vts[2] = &model.Nodes()[v3 - 1];
                vts[3] = &model.Nodes()[v4 - 1];
                vts[4] = &model.Nodes()[v5 - 1];
                vts[5] = &model.Nodes()[v6 - 1];
                vts[6] = &model.Nodes()[v7 - 1];
                vts[7] = &model.Nodes()[v8 - 1];

                // Характеристики элемента
                bool is_surface = true;
                if (it_h > 0) {is_surface = false;}
                const Material* material = layer.get_material(it_h);
                // const Material* material = &AMg_6;
                std::string* layer_name = layer.get_name(it_h);
                std::string* primitive_name = geom.get_name(it_x);
                
                // Создание элемента
                model.add_element(ElementType::LQube, vts, nbr, material, is_surface, 1.0, layer_name, primitive_name);
                ++nbr;

                v1 = next_phi(v1);
            }
            v1 = next_h(v1);
        }
        base = next_x(base);
    }

    // Днище
    vts.resize(6);
    base = model.Nodes().size() - layer.FRNT;
    for (int it_h = 0; it_h < layer.FRNT; ++it_h)
    {
        v1 = base;
        v2 = bot_iter(v1);
        for (int it_p = 0; it_p < c_phi; ++it_p)
        {
            // Формируем массив вершин
            v3 = next_phi(v2);
            v4 = next_h(v1);
            v5 = next_h(v2);
            v6 = next_h(v3);
            vts[0] = &model.Nodes()[v1 - 1];
            vts[1] = &model.Nodes()[v2 - 1];
            vts[2] = &model.Nodes()[v3 - 1];
            vts[3] = &model.Nodes()[v4 - 1];
            vts[4] = &model.Nodes()[v5 - 1];
            vts[5] = &model.Nodes()[v6 - 1];

            // Характеристики элемента
            bool is_surface = true;
            if (it_h > 0) {is_surface = false;}
            const Material* material = layer.get_material(it_h);
            // const Material* material = &AMg_6;
            std::string* layer_name = layer.get_name(it_h);
            std::string* primitive_name = &geom.names[4];
            
            // Создание элемента
            model.add_element(ElementType::LWedge, vts, nbr, material, is_surface, 1.0, layer_name, primitive_name);
            ++nbr;

            v2 = next_phi(v2);
        }
        // base = next_h(base);
        base += 1;
    }
}