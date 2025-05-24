#include <sstream>
#include <array>
#include <stdexcept>

template<std::size_t N>
std::array<double, N> parse_doubles(const std::string& s) {
    std::array<double, N> values{};
    std::stringstream ss(s);
    std::string token;
    std::size_t i = 0;

    while (std::getline(ss, token, ',')) {
        if (i >= N) throw std::runtime_error("Too many thickness values");
        values[i++] = std::stod(token);
    }
    if (i != N) throw std::runtime_error("Expected exactly " + std::to_string(N) + " thickness values");
    return values;
}

template<std::size_t N>
std::array<int, N> parse_ints(const std::string& s) {
    std::array<int, N> values{};
    std::stringstream ss(s);
    std::string token;
    std::size_t i = 0;

    while (std::getline(ss, token, ',')) {
        if (i >= N) throw std::runtime_error("Too many element values");
        values[i++] = std::stoi(token);
    }
    if (i != N) throw std::runtime_error("Expected exactly " + std::to_string(N) + " element values");
    return values;
}
