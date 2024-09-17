#include "../../toolbox/eigen-3.4.0/Eigen/Dense"
#include <iostream>

int main() {
    Eigen::Vector3d a;
    a << 1.1, 2.2, 3.3;
    std::cout << a;
    return 0;
}