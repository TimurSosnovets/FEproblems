#include  "../../headers/thermal/Materials.hpp"

// Линейная интерполяция (два вектора)
double interp_lin(const std::vector<double>& temperatures, const std::vector<double>& values, double T)
{
    if (temperatures.size() != values.size() || temperatures.empty() || values.empty()) {
        throw std::invalid_argument("Temperature and lambda arrays must have the same non-zero size");
    }

    auto it = std::lower_bound(temperatures.begin(), temperatures.end(), T);

    if (it == temperatures.begin()) return values.front();
    if (it == temperatures.end()) return values.back();

    size_t idx = std::distance(temperatures.begin(), it);
    double T1 = temperatures[idx - 1], T2 = temperatures[idx];
    double val1 = values[idx - 1], val2 = values[idx];

    return val1 + (T - T1) * (val2 - val1) / (T2 - T1);
}

// Линейная интерполяция (один вектор пар)
double interp_lin(const std::vector<std::pair<double, double>>& data, double T)
{
    if (data.empty()) {
        throw std::invalid_argument("Temperature and lambda arrays must have the same non-zero size");
    }

    // Итератор
    auto it = std::lower_bound(data.begin(), data.end(), T, 
    [](const std::pair<double, double>& entry, double value) {return entry.first < value;} );

    if (it == data.begin()) return data.front().second;
    if (T > data.back().first) return data.back().second;
    if (it == data.end()) return std::prev(it)->second;

    
    double T1 = std::prev(it)->first, T2 = it->first;
    double val1 = std::prev(it)->second, val2 = it->second;

    return val1 + (T - T1) * (val2 - val1) / (T2 - T1);
}


// Конструктор для непостоянных параметров
Material::Material(std::vector<std::pair<double, double>> tcc, std::vector<std::pair<double, double>> shc, double r) :
    TCC(tcc), SHC(shc), rho(r), const_prop(false) {};

// Конструктор для постоянных параметров
Material::Material(double tcc, double shc, double r) : TCC({{0, tcc}}), SHC({{0, shc}}), rho(r), const_prop(true) {};

// Коэффициент теплопроводности при заданной температуре
double Material::get_TCC(double T) const
{
    if (const_prop) { return TCC[0].second; }
        else { return interp_lin(TCC, T); } 
}

// Удельная теплоёмкость при заданной температуре
double Material::get_SHC(double T) const 
{
    if (const_prop) { return SHC[0].second; }
        else { return interp_lin(SHC, T); }
} 

// Плотность
double Material::dens() const 
{
    return rho;
}


// СУ-2500
const Material GC_2500
    (
        {{300.0, 7.0}, {400.0, 7.2}, {500.0, 7.5}, {600.0, 7.7}, {700.0, 8.0}, {800.0, 8.3}, {900.0, 8.5}, {1000.0, 9.0}, {1500.0, 10.0}, {2000.0, 11.0}, {2500.0, 11.5}}, 
        {{300.0, 0.678}, {400.0, 1.002}, {500.0, 1.223}, {600.0, 1.397}, {800.0, 1.534}, {1000.0, 1.8}, {1200.0, 1.907}, {1400.0, 1.982}, {1600.0, 2.036}, {1800.0, 2.077}, {2000.0, 2.108}, {2200.0, 2.132}, {2400.0, 2.152}},
        1485
    );

// ТЗМК-10
const Material TZMK_10
    (
        {{273.0, 0.021}, {373.0, 0.029}, {473.0, 0.043}, {573.0, 0.059}, {673.0, 0.066}, {773.0, 0.075}, {873.0, 0.081}, {973.0, 0.091}, {1073.0, 0.104}, {1173.0, 0.118}, {1273.0, 0.132}, {1373.0, 0.147}, {1473.0, 0.164}, {1573.0, 0.184}}, 
        {{273.0, 0.693}, {373.0, 0.841}, {473.0, 0.95}, {573.0, 1.02}, {673.0, 1.089}, {773.0, 1.127}, {873.0, 1.151}, {973.0, 1.18}, {1073.0, 1.197}, {1173.0, 1.21}, {1273.0, 1.223}, {1373.0, 1.245}, {1473.0, 1.269}, {1573.0, 1.3}}, 
        143
    );

// АМг-6
const Material AMg_6 (122.0, 922.0, 2640.0);


