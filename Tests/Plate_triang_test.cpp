#include "../Plates/Triangulated.h"
#include <iostream>
#include <cassert>
#include <cmath>


// Function to compare matrices
void compare_matrices(const Eigen::Matrix<double, 6, 6>& mat1, const Eigen::Matrix<double, 6, 6>& mat2, double tol = 1e-6) {
    assert((mat1 - mat2).norm() < tol);
}

void test_triangle_fe() {
    // Define test data
    std::vector<std::pair<Point, unsigned int>> vertices = {
        {Point(0.0, 0.0), 0},
        {Point(1.0, 0.0), 1},
        {Point(0.0, 1.0), 2}
    };
    Material material(210e9, 0.3);
    double thickness = 1.0;

    // Create TriangleFE object
    TriangleFE triangle(vertices, material, thickness);

    // Expected strain-displacement matrix _B
    Eigen::Matrix<double, 3, 6> expected_B;
    expected_B << -1, 0, 1, 0, 0, 0,
                  0, -1, 0, 1, 0, 1,
                  1, -1, -1, 1, -1, 0;

    // Compute expected stiffness matrix _K
    Eigen::Matrix<double, 6, 6> expected_K;
    Eigen::Matrix<double, 3, 6> B = expected_B;
    Eigen::Matrix3d D = material.D();
    double A = 0.5;  // Area of the triangle
    double h = 1.0;
    expected_K = 0.25 * B.transpose() * D * B;

    // Compare computed matrices with expected values
    compare_matrices(triangle.B(), expected_B);
    compare_matrices(triangle.K(), expected_K);

    std::cout << "All tests passed!" << std::endl;
}

int main() {
    test_triangle_fe();
    return 0;
}
