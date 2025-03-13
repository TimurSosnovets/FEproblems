#include "Output.hpp"
#include "TFE_model.hpp"

std::vector<int> output_nodes; // Номера узлов, на которые нужно будет смотреть
void make_model(TFE_model& model, const std::array<float, 3> dimentions, const std::array<int, 3> mesh)
{
    /*Создание КЭ модели пластины (параллеллепипеда) по размерам*/
    // Инициализация
    double x = 0, y = 0, z = 0;
    const float lx = dimentions[0], ly = dimentions[1], lz = dimentions[2];
    const double dx = dimentions[0] / mesh[0], dy = dimentions[1] / mesh[1], dz = dimentions[2] / mesh[2];
    int node_number = 1;
    int element_number = 1;
    std::vector<const Node*> vertices(8);
    double eps = 1e-6; // Для сравнения координат

    // Создание узлов
    for (int step_y = 0; (step_y) < mesh[1] + 1; ++step_y)
    {
        y = step_y * dy;
        for (int step_z = 0; (step_z) < mesh[2] + 1; ++step_z)
        {
            z = step_z * dz;
            for (int step_x = 0; (step_x) < mesh[0] + 1; ++step_x)
            {
                x = step_x * dx;
                model.add_node(Point(x, y, z), node_number);
                if ((x < eps) && (z < eps)) {output_nodes.emplace_back(node_number - 1);} // Узлы в плоскости XoZ
                ++node_number;
            }
        }
    }

    // Номера следующего узла по соответствующим осям
    auto next_x = [mesh] (int node) -> int {return node + 1;};
    auto next_z = [mesh] (int node) -> int {return node + (mesh[0] + 1);};
    auto next_y = [mesh] (int node) -> int {return node + (mesh[0] + 1) * (mesh[2] + 1);};

    for (const auto& node : model.Nodes())
    {
        std::cout << "Node: " << node.global_number() << " X = " << node.coords().x << ", Y = " << node.coords().y << ", Z = " << node.coords().z << ".\n"; 
        std::cout << "next_x " << next_x(node.global_number()) << std::endl;
        std::cout << "next_z " << next_z(node.global_number()) << std::endl;
        std::cout << "next_y " << next_y(node.global_number()) << std::endl << std::endl;
    }
    std::cin.get();

    // Создание элементов
    for (const auto& node : model.Nodes())
    {
        // Учитываем только базовые узлы
        if (abs(node.coords().y - ly) < eps) {break;}
        if ((abs(node.coords().x - lx) < eps) || (abs(node.coords().z - lz) < eps)) {continue;}

        // Создание элемента
        int v1 = node.global_number() - 1;
        int v2 = next_x(v1);
        int v3 = next_z(v2);
        int v4 = next_z(v1);
        int v5 = next_y(v1);
        int v6 = next_y(v2);
        int v7 = next_y(v3);
        int v8 = next_y(v4);
        vertices[0] = &model.Nodes()[v1];
        vertices[1] = &model.Nodes()[v2];
        vertices[2] = &model.Nodes()[v3];
        vertices[3] = &model.Nodes()[v4];
        vertices[4] = &model.Nodes()[v5];
        vertices[5] = &model.Nodes()[v6];
        vertices[6] = &model.Nodes()[v7];
        vertices[7] = &model.Nodes()[v8];
        model.add_element(ElementType::LQube, vertices, element_number, &Steel_30HGSA);
        ++element_number;
    }
}

int main()
{
    std::string message;
    /*Взятие значений*/
    logger::log("Mesh test");
    std::array<float, 3> dim = {0.01, 0.05, 0.1};
    std::array<int, 3> fec;
    logger::log("Enter number of elements for each side of plate:");
    std::cout << "OX: ";
    std::cin >> fec[0];
    std::cout << "OY: ";
    std::cin >> fec[1];
    std::cout << "OZ: ";
    std::cin >> fec[2];
    std::cout << std::endl;

    TFE_model model(fec[0], fec[1], fec[2]);
    make_model(model, dim, fec);
    std::cin.get();
    model.mesh_info();
    std::cin.get();
    model.pre_calculate();
    
    /*Решение*/
    // Ввод значений
    double constraint_temp, initial_temp;
    int time;
    float time_step;
    int constraint_count = (fec[0] + 1) * (fec[2] + 1) * 2;
    std::vector<std::pair<int, double>> LBC;

    logger::log("Enter constraint temperature (K):");
    std::cin >> constraint_temp;

    // Формирование вектора закреплений
        // Левая грань
    for (int i = 0; i < constraint_count / 2; ++i)
    {
        LBC.emplace_back(i, constraint_temp);
    }
        // Правая грань
    for (int i = model.Nodes().size() - constraint_count / 2; i < model.Nodes().size(); ++i)
    {
        LBC.emplace_back(i, constraint_temp);
    }
    /*Расчёт*/
    auto results = model.transient_analisys(LBC, 0.0);

    /*Вывод результатов*/
    message = "Temperatures across time for node ";
    for (const auto& n_res : results.NT_samples)
    {
        logger::log(message + std::to_string(n_res.first) + ":");
        for (const auto& temp : n_res.second)
        {
            std::cout << temp << std::endl;
        }
    }
    std::cout << std::endl;
    
    std::vector<Eigen::VectorXd> thickness_distribution;
    thickness_distribution.reserve(results.VNT_samples.size());
    for (const auto& dist : results.VNT_samples)
    {   
        // std::cout << "/nReserving for t = " << dist.first;
        Eigen::VectorXd temp = Eigen::VectorXd::Zero(output_nodes.size());
        for (int i = 0; i < output_nodes.size(); ++i)
        {
            temp(i) = dist.second[output_nodes[i]];
        }
        thickness_distribution.emplace_back(temp);
        // std::cout << " -> size = " << thickness_distribution.size();
    }
    int i = 0;
    

    message = "Thickness distribution temperatures in time ";
    for (const auto& vnt_res : results.VNT_samples)
    {
        logger::log(message + std::to_string(vnt_res.first) + " s:");
        std::cout << thickness_distribution[i] << std::endl << std::endl;
        ++i;
    }


    std::cout << "Calculation has ended. Press enter to escape...";
    std::cin.get();

    return 0;
}