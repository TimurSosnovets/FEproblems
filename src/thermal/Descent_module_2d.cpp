#include "../../headers/thermal/Descent_module_2d.hpp"
#include <string>


// ГЕОМЕТРИЯ СПУСКАЕМОГО АППАРАТА
// Конструктор класса
DM_Geom2d::DM_Geom2d(const double R_cr, const double R_c1, const double R_c2, const double X_cr, const double X_c1, const double X_c2, const double X_cl)
 : R_circle(R_cr), R_cone1(R_c1), R_cone2(R_c2), X_circle(X_cr), X_cone1(X_c1), X_cone2(X_c2), X_cyl(X_cl) {};

// Х-координата окружности
double DM_Geom2d::circle(const double x, const double r, const double center) 
{
    return -sqrt(pow(r, 2) - pow((x - center), 2)); 
}

// X-координата прямой, проходящей через две точки
double DM_Geom2d::line(const double x, const Point P1, const Point P2)
{
    return (((P2.y - P1.y) / (P2.x - P1.x)) * (x - P1.x) + P1.y);
}

// Возвращаемые значения
const double DM_Geom2d::x_cr() const
{
    return X_circle;
}

const double DM_Geom2d::x_cn1() const
{
    return X_cone1;
}

const double DM_Geom2d::x_cn2() const
{
    return X_cone2;
}

const double DM_Geom2d::x_cl() const
{
    return X_cyl;
}

const double DM_Geom2d::r_cr() const
{
    return R_circle;
}

const double DM_Geom2d::r_cn1() const
{
    return R_cone1;
}

const double DM_Geom2d::r_cn2() const
{
    return R_cone2;
}

// КОНЕЧНОЭЛЕМЕНТНАЯ МОДЕЛЬ СПУСКАЕМОГО АППАРАТА
enum OuterState {CIRCLE, FIRST_CONE, SECOND_CONE, CYLINDER}; // Итерации по длине
enum InnerState {GLASSY_CARBON, TZMK, AMg_6}; // Итерации по слоям

// Разбиение на узлы
void DM_FEmodel::nodes_creation(std::vector<std::pair<Point, int>>& Nodes, const int N_cr, const int N_cn1, const int N_cn2, const int N_cl, const int N_gc, const int N_tzmk)
{
    Nodes.reserve(_DOF);
    int nbr = 1; // Номер узла
    const double step_CR = geometry.x_cr() / N_cr, step_CN1 = (geometry.x_cn1() - geometry.x_cr()) / N_cn1, step_CN2 = (geometry.x_cn2() - geometry.x_cn1()) / N_cn2, step_CL = (geometry.x_cl() - geometry.x_cn2()) / N_cl; // Шаг по длине
    const double step_GC = h_GC / N_gc, step_TZMK = h_TZMK / N_tzmk; // Шаг по слою
    double x, y; // Координаты точки 
    double h; // Глубина относительно поверхности
    std::optional<double> is_circle;

    std::function<double(std::optional<double>)> x_coord; // Вычисление x-координаты узла
    std::function<double(double, double)> y_coord; // Вычисление y-координаты узла
    
    OuterState OcurrentSt = CIRCLE;
    InnerState IcurrentSt = GLASSY_CARBON;
    std::cout << "CIRCLE\n";

    for (int OX = 0; OX < N_cr + N_cn1 + N_cn2 + N_cl +1; ++OX) 
    {
        switch (OcurrentSt) // Выбирается способ расчета координат, в зависимости от кривой
        {
            case CIRCLE:
                x_coord = [this, OX, N_cr, step_CR](std::optional<double> X_origin) 
                { return X_origin.value() + (step_CR - X_origin.value() / N_cr) * OX; };
                y_coord = [this](double x, double h) { return DM_Geom2d::circle(x, geometry.r_cr() - h); };
            break;

            case FIRST_CONE:
                x_coord = [this, OX, N_cr, N_cn1, step_CN1](std::optional<double> X_origin) 
                { return geometry.x_cr() + step_CN1 * (OX - N_cr); };
                y_coord = [this](double x, double h) { return DM_Geom2d::line(x, Point(geometry.x_cr(), -(geometry.r_cr() - h / cos(21 / 180.0 * M_PI))), Point(geometry.x_cn1(), -(geometry.r_cn1() - h / cos(21 / 180.0 * M_PI)))); };
            break;

            case SECOND_CONE:
                x_coord = [this, OX, N_cr, N_cn1, N_cn2, step_CN2](std::optional<double> X_origin) 
                { return geometry.x_cn1() + step_CN2 * (OX - N_cr - N_cn1); };
                y_coord = [this](double x, double h) { return DM_Geom2d::line(x, Point(geometry.x_cn1(), -(geometry.r_cn1() - h / cos(8 / 180.0 * M_PI))), Point(geometry.x_cn2(), -(geometry.r_cn2() - h / cos(8 / 180.0 * M_PI)))); };
            break;

            case CYLINDER:
                x_coord = [this, OX, N_cr, N_cn1, N_cn2, N_cl, step_CL](std::optional<double> X_origin) 
                { return geometry.x_cn2() + step_CL * (OX - N_cr - N_cn1 - N_cn2); };
                y_coord = [this](double x, double h) { return DM_Geom2d::line(x, Point(geometry.x_cn2(), -(geometry.r_cn2() - h)), Point(geometry.x_cl(), -(geometry.r_cn2() - h))); };
            break;
        }

        // Шаги по слоям
        IcurrentSt = GLASSY_CARBON;
        for (int OR = 0; OR < N_gc + N_tzmk + 1 + 1; ++OR)
        {
            switch (IcurrentSt) 
            {                
                case GLASSY_CARBON:
                h = step_GC * OR;
                is_circle = h;
                if (OR == N_gc) {IcurrentSt = TZMK;}
                break;

                case TZMK:
                h = h_GC + step_TZMK * (OR - N_gc);
                is_circle = h;
                if (OR == N_gc + N_tzmk) {IcurrentSt = AMg_6;}
                break;

                case AMg_6:
                h = h_GC + h_TZMK + h_AMg;
                is_circle = h;
                break;
            }  

            x = x_coord(is_circle);
            y = y_coord(x, h);                
            Nodes.emplace_back(std::make_pair(Point(x, y), nbr));
            //std::cout << "Number " << nbr << ". " << "OX = " << OX << ", OR = " << OR << ";   x = " << x << ", y = " << y << ".\n";
            ++nbr;
        }

        switch (OcurrentSt)
        {
            case CIRCLE:
            if (OX == N_cr) {OcurrentSt = FIRST_CONE; is_circle = std::nullopt; std::cout << "FIRST CONE\n"; };
            break;

            case FIRST_CONE:
            if (OX == N_cr + N_cn1) {OcurrentSt = SECOND_CONE; std::cout << "SECOND CONE\n"; };
            break;
            
            case SECOND_CONE:
            if (OX == N_cr + N_cn1 + N_cn2) {OcurrentSt = CYLINDER; std::cout << "CYLINDER\n"; };
            break;
        }
    }
};

// Создание элементов
void DM_FEmodel::elements_creation(const int N_cr, const int N_cn1, const int N_cn2, const int N_cl, const int N_gc, const int N_tzmk)
{
    // Глобальный номер левой нижней вершины элемента
    std::function<int(int, int)> k;
    k = [this, N_gc, N_tzmk](int i, int j) 
                { return i * (N_gc + N_tzmk + 1 + 1) + j; }; // количество эл-то вдоль слоя + 1;
    
    int n_elem = 0; // Номер КЭ
    double lambda; // Коэффициент теплопроводности
    std::string mat;

    std::cout << "\n\n\n Elements:\n";
    for (int i = 0; i < N_cr + N_cn1 + N_cn2 + N_cl; ++i) // Шаг по длине 
    {
        InnerState IcurrentSt = GLASSY_CARBON;
        mat = "Glassy carbon";
        for (int j = 0; j < N_gc + N_tzmk + 1; ++j) // Шаг по слоям
        {
            switch (IcurrentSt)
            {
                case GLASSY_CARBON:
                lambda = Lamda_GC;
                if (j == N_gc) {IcurrentSt = TZMK; mat = "TZMK"; }
                break;

                case TZMK:
                lambda = Lambda_TZMK;
                if (j == N_gc + N_tzmk) {IcurrentSt = AMg_6; mat = "AMg6"; }
                break;

                case AMg_6:
                lambda = Lambda_AMg;
                break;
            }
            //std::cout << "i = " << i << ", j = " << j << "; k = " << k(i,j) << std::endl;
            _FEs[n_elem] = {LQuad({_Nodes[k(i, j)], _Nodes[k(i+1, j)], _Nodes[k(i+1, j+1)], _Nodes[k(i, j+1)]}, lambda, lambda), n_elem + 1};
            //std::cout << "Number " << n_elem + 1 << ": " << mat << ", " <<   _FEs[n_elem].value().first.Vertices()[0].get().second << ", " << _FEs[n_elem].value().first.Vertices()[1].get().second << ", " <<  _FEs[n_elem].value().first.Vertices()[2].get().second << ", " << _FEs[n_elem].value().first.Vertices()[3].get().second << "." << std::endl;
            ++n_elem; 
        }
    }
}

// Конструктор класса
DM_FEmodel::DM_FEmodel(DM_Geom2d& geom, const double GC, const double TZMK, const double AMg, const int N_cr, const int N_cn1, const int N_cn2, const int N_cl, const int N_gc, const int N_tzmk) 
: geometry(geom), h_GC (GC), h_TZMK(TZMK), h_AMg(AMg), _DOF((N_cr + N_cn1 + N_cn2 + N_cl + 1) * (N_gc + N_tzmk + 2))
{
    nodes_creation(_Nodes, N_cr, N_cn1, N_cn2, N_cl, N_gc, N_tzmk);
    elements_creation(N_cr, N_cn1, N_cn2, N_cl, N_gc, N_tzmk);
    Assembly<LQuad, MAX_DOF>(_GCM, _DOF, &_FEs);
    //std::cout << "\n\n\n\nGCM: \n" << _GCM; 
};

// Возвращаемые значения
const std::vector<std::pair<Point, int>>& DM_FEmodel::Nodes() const
{
    return _Nodes;
}

const std::array<std::optional<std::pair<LQuad, int>>, MAX_DOF>& DM_FEmodel::FEs() const
{
    return _FEs;
}

const Eigen::MatrixXd& DM_FEmodel::GCM() const
{
    return _GCM;
}

