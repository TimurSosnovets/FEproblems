#include "../Plates/Triangulated.h"
#include "../Plates/Output_txt.h"
#include <iostream>
#include <cassert>
#include <cmath>
#include "../Utils/parser.h"


// Функция сравнения матриц
void compare_matrices(const Eigen::MatrixXd& mat1, const Eigen::MatrixXd& mat2, double tol = 1e-6) {
    assert((mat1 - mat2).norm() < tol);
}

// Материалы 
Material Steel(2e11, 0.3), AMg6(7e10, 0.33);
Material Test_mat(10000000000, 0.33);

// Тест создания треугольного элемента
void test_triangle_fe() {
    // Define test data
    std::vector<std::pair<Point, unsigned int>> vertices = {
        {Point(0.0, 0.0), 0},
        {Point(0.0, 1.0), 1},
        {Point(1.0, 0.0), 2}
    };
    
    double thickness = 1.0;

    // Create TriangleFE object
    TriangleFE triangle(vertices, Steel, thickness);

    // Expected strain-displacement matrix _B
    Eigen::Matrix<double, 3, 6> expected_B;
    expected_B << -1, 0, 1, 0, 0, 0,
                  0, -1, 0, 0, 0, 1,
                  -1, -1, 0, 1, 1, 0;

    // Compute expected stiffness matrix _K
    Eigen::Matrix<double, 6, 6> expected_K;
    Eigen::Matrix<double, 3, 6> B = expected_B;
    Eigen::Matrix3d D = Steel.D();
    double A = 0.5;  // Area of the triangle
    double h = 1.0;
    expected_K = A * h * B.transpose() * D * B;

   
    std::cout << std::endl << "B = \n" << triangle.B() << std::endl;
    std::cout << std::endl << "expected B = \n" << expected_B << std::endl;
    compare_matrices(triangle.B(), expected_B);

    std::cout << std::endl << "K = \n" << triangle.K() << std::endl;
    std::cout << std::endl << "expected K = \n" << expected_K << std::endl;
    compare_matrices(triangle.K(), expected_K);
        
    std::cout << "Triangle_FE: All tests passed!" << std::endl;
}

void test_plate_triangulated() {
    
}

int main() {
   // test_triangle_fe();    
    Plate_triangulated Plate(2, 2, 6, 6, 0.1, Test_mat);

    std::vector<std::tuple<int, bool, bool>> LBC_dof;
    std::vector<std::tuple<int, double, double>> LBC_force;

    Parser::parse_file("Plate_LBC/Plate_LBC_test.txt", LBC_dof, LBC_force);

    //std::cout << "dof size " << LBC_dof.size() << "   force size " << LBC_force.size() << std::endl; 

    auto Plate_solution = Plate.solve(LBC_dof, LBC_force);
    // std::cout << "Nodal displacements: \n" <<std::get<0>(Plate_solution);
    std::string filename = "Plate_Femap.txt";
    solution_txt(Plate_solution, 5, filename);
    return 0;
}
