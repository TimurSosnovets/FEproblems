#include "TFE_model.hpp"
#include "Output.hpp"
#include "Vec2D.hpp"
#include <cmath>

// Градусы в радианы
double deg2rad(double deg)
{
    return (deg / 180) * M_PI;
}

// Y-координата в плоскости xOy (Сфера)
double r_circle(double x, double h, double R_sphere)
{
    return sqrt( pow(R_sphere - h, 2) - pow(x - R_sphere, 2) );
}

// Y-координата в плоскости xOy (Прямая)
double r_line(double x, double h, Point base1, Point base2)
{
    double x1 = base1.x, x2 = base2.x, y1 = base1.y, y2 = base2.y;
    // Вектор смещения по нормали
    Vec2D V(x2 - x1, y2 - y1);
    Vec2D normal = V.perpendicular();
    normal.set_length(h);
    // Новые точки (смещенные по нормали)
    std::pair<double, double> new_base1 = normal.p2p({x1, y1}), new_base2 = normal.p2p({x2, y2});
    
    return new_base1.second + (new_base2.second - new_base1.second) / (new_base2.first - new_base1.first) * x;
}

// Модель слоёв
struct Layers
{
    std::array<double, 3> thickness;
    std::array<int, 3> fragment;
    std::array<double, 3> step;
    std::array<std::string, 3> names = {"GC-2500", "TZMK-10", "AMg6"};
    double THCK;
    int FRNT;

    Layers(std::array<double, 3> t, std::array<int, 3> f) : thickness(t), fragment(f)
    {
        THCK = std::accumulate(thickness.begin(), thickness.end(), 0.0);
        FRNT = std::accumulate(fragment.begin(), fragment.end(), 0);
        for (int i = 0; i < 3; ++i) {step[i] = thickness[i] / fragment[i];}
    }
    
    double depth(int iter) const
    {
        if (iter < fragment[0]) {return iter * step[0];}
        else if (iter < fragment[0] + fragment[1]) {return thickness[0] + (iter - fragment[0]) * step[1];}
        else return thickness[0] + thickness[1] + (iter - fragment[0] - fragment[1]) * step[2];
    }

    const Material* get_material(int iter) const
    {
        if (iter < fragment[0]) {return &GC_2500;}
        else if (iter < fragment[0] + fragment[1]) {return &TZMK_10;}
        else return &AMg_6;
    }

    std::string* get_name(int iter)
    {
        if (iter < fragment[0]) {return &names[0];}
        else if (iter < fragment[0] + fragment[1]) {return &names[1];}
        else return &names[2];
    }
};

// Модель геометрии
struct Geometry
{
    // Геометрия
    double R_sphere;
    std::array<double, 4> x_refers = {R_sphere * (1 - cos(deg2rad(69))), (10000 - 4800 - 2320) / 1000, (10000 - 2320) / 1000, (10000) / 1000};
    std::array<double, 4> r_refers = {R_sphere, R_sphere + x_refers[1] * atan(deg2rad(69)), 4.0 - 4.8 * atan(deg2rad(82)), 4.0};
    std::array<std::string, 5> names = {"Sphere", "First cone", "Second cone", "Cylinder", "Bottom"};

    // КЭ разбиение
    int FE_sph, FE_cone1, FE_cone2, FE_cyl, FE_all;
    double step_sph, step_cone1, step_cone2, step_cyl;

    std::string* get_name(int iter)
    {
        if (iter < FE_sph) {return &names[0];}
        else if (iter < FE_cone1) {return &names[1];}
        else if (iter < FE_cone2) {return &names[2];}
        else if (iter < FE_cyl) {return &names[3];}
        else return &names[4];
    }
};

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
        ++nbr;
    }

    // // СФЕРА - основные узлы
    // for (int it_x = 1; it_x < c_x + 1; ++it_x)
    // {
    //     for (int it_phi = 0; it_phi < c_phi; ++it_phi)
    //     {
    //         for (int it_h = 0; it_h < layer.FRNT + 1; ++it_h)
    //         {
    //             h = layer.depth(it_h);
    //             d_x = (x_sph - h) / c_x;
    //             x = h + it_x * d_x;
    //             r = r_circle(x, h, R_sph);
    //             phi = it_phi * d_phi;
    //             y = (-1) * r * cos(phi);
    //             z = r * sin(phi);

    //             model.add_node(Point(x, y, z), nbr);
    //             ++nbr;
    //         }
    //     }
    // }

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
    for (int it_x = 0; it_x <= geom.FE_cone1; ++it_x)
    {
        for (int it_phi = 0; it_phi < c_phi; ++it_phi)
        {
            for (int it_h = 0; it_h <= layer.FRNT; ++it_h)
            {
                h = layer.depth(it_h);
                d_x = (geom.x_refers[1]- geom.x_refers[0]) / geom.FE_cone1;
                x = geom.x_refers[0] + it_x * d_x;
                r = 
                phi = it_phi * d_phi;
                y = (-1) * r * cos(phi);
                z = r * sin(phi);

                model.add_node(Point(x, y, z), nbr);
                ++nbr;
            }
        }
    }
    // Номера соседних узлов
    auto next_x = [layer, c_x, c_phi] (int node) -> int 
    {
        if (node <= layer.FRNT + 1) {return  node + layer.FRNT + 1;}
        else {return node + (layer.FRNT + 1) * c_phi;}
    };
    auto next_h = [layer, c_x, c_phi] (int node) -> int {return node + 1;};
    auto next_phi = [layer, c_x, c_phi] (int node) -> int 
    {
        if (node <= layer.FRNT + 1) {return node;}

        int rem = (node - (layer.FRNT + 1)) % ((layer.FRNT + 1) * (c_phi));
        if ((rem > (c_phi - 1) * (layer.FRNT + 1)) || (rem == 0)) {return node - (layer.FRNT + 1) * (c_phi - 1);}

        return node + layer.FRNT + 1;
    };
    
    // Вывод информации об узлах
    for (const auto& node : model.Nodes())
    {
        std::cout << std::fixed << std::setprecision(2) 
        << "Node: " << node.global_number() 
        << " X = " << node.coords().x * 1000 << ", Y = " << node.coords().y * 1000 << ", Z = " << node.coords().z * 1000<< ".\n"; 
        std::cout << "next_x " << next_x(node.global_number()) << std::endl;
        std::cout << "next_h " << next_h(node.global_number()) << std::endl;
        std::cout << "next_phi " << next_phi(node.global_number()) << std::endl << std::endl;
    }
    
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
        for (int it_p = 0; it_p < c_phi; ++it_p)
        {
            // Формируем массив вершин
            v2 = next_x(v1);
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
            // const Material* material = layer.get_material(it_h);
            const Material* material = &AMg_6;
            std::string* layer_name = layer.get_name(it_h);
            
            // Создание элемента
            model.add_element(ElementType::LWedge, vts, nbr, material, is_surface, 1.0, layer_name);
            ++nbr;

            v1 = next_phi(v1);
        }
        base = next_h(base);
    }

    // Остальная сфера
    vts.resize(8);
    base = next_x(1);
    for (int it_x = 1; it_x < c_x; ++it_x)
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
                // const Material* material = layer.get_material(it_h);
                const Material* material = &AMg_6;
                std::string* layer_name = layer.get_name(it_h);
                
                // Создание элемента
                model.add_element(ElementType::LQube, vts, nbr, material, is_surface, 1.0, layer_name);
                ++nbr;

                v1 = next_phi(v1);
            }
            v1 = next_h(v1);
        }
        base = next_x(base);
    }
}

int main()
{
    /*Геометрия*/
    Layers layer({0.015, 0.045, 0.002}, {1, 1, 1});

    /*КЭ модель*/
    TFE_model DM_FE(10, 10, 10);
    
    // Заполнение массива узлов
    make_model(DM_FE, layer, 2, 4);
    logger::log("Model has been made successfully!");
    std::cin.get();

    DM_FE.mesh_info();
    std::cin.get();

    DM_FE.mesh_check();
    logger::log("Successful mesh check!");

    /*Решение*/
    // Ввод значений
    double constraint_temp = 300;
    std::vector<std::pair<int, double>> LBC;
    for (const auto& node : DM_FE.Nodes())
    {
        if (node.global_number() % (layer.FRNT + 1) == 0) {LBC.emplace_back(node.global_number() - 1, constraint_temp);}
    }

    Eigen::VectorXd temps = DM_FE.steady_state_analysis(LBC, 1e5, true);
    
    logger::log("Nodal temperatures:");
    std::cout << temps;
    std::cin.get();
    return 0;

}
