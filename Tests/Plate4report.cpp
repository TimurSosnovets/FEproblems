#include "../headers/mechanical/Plate_types.hpp"
#include "../headers/mechanical/Output_txt.hpp"
#include "../headers/mechanical/parser.hpp"


Isotropic Steel(2e11, 0.3, 13e-6);
int main() {  
    Plate_triangulated Plate(0.15, 0.2, 3, 5, 0.001, Steel);
    
    LBC LBC;    
    Parser::parse_file("Plate_LBC/Plate4report_LBC.txt", LBC);
    
    auto Plate_solution = solve(Plate, LBC);
    std::string filename = "Plate4report_result.txt";
    solution_txt(Plate_solution, 7, filename);
    return 0;
}