#include "../Plates/Triangulated.h"
#include "../Plates/Output_txt.h"
#include <iostream>
#include <cassert>
#include <cmath>
#include "../Utils/parser.h"

// Материалы 
Material Steel(2e11, 0.3), AMg6(7e10, 0.33);
Material Test_mat(10000000000, 0.33);

int main() {
   // test_triangle_fe();    
    Plate_triangulated Plate(2, 2, 1, 1, 0.1, Steel);

    std::vector<std::tuple<int, bool, bool>> LBC_dof;
    std::vector<std::tuple<int, double, double>> LBC_force;

    Parser::parse_file("Plate_simple_LBC.txt", LBC_dof, LBC_force);
    auto Plate_solution = Plate.solve(LBC_dof, LBC_force);
    
    std::string filename = "Plate_simple_solution.txt";
    solution_txt(Plate_solution, 5, filename);
    return 0;
}