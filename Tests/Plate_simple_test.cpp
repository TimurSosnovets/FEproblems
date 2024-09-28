#include "../headers/mechanical/Plate_types.hpp"
#include "../headers/mechanical/Output_txt.hpp"
#include "../headers/mechanical/parser.hpp"
#include <cassert>

Isotropic Steel(2e11, 0.3, 13e-6), AMg6(7e10, 0.33, 24.7e-6);
int main() {  
    Plate_triangulated Plate(1, 1, 1, 1, 0.01, Steel);
    // int i = 1;
    // for (const auto& FE : Plate.FEs()){
    //     std::cout << "Element " << i << ".\n";
    //     for (const auto& vert : FE.first.verts()) {
    //         std::cout << "Vertex " << vert.second << ": (" << vert.first.coord_x() << ", " << vert.first.coord_y() << ")\n";
    //     }
    //     std::cout << "B = \n" << FE.first.B() << "\n\n";
    //     std::cout <<  "SM = \n" << FE.first.K() << "\n\n\n";
    //     ++i;
    // }
    // std::cout << "\n GSM = \n" << Plate.GSM(); 

    LBC LBC;    
    Parser::parse_file("Plate_LBC/PlateSimple_LBC_temp.txt", LBC);
    // Дебаж
    for (const auto& Ndl_T : LBC.Nodal_Temp) {
        std::cout << "Nodal_Temp: Node " << Ndl_T.first << " Temp " << Ndl_T.second << std::endl;
    }

    auto Plate_solution = solve(Plate, LBC);
    std::string filename = "PlateSimple_result.txt";
    solution_txt(Plate_solution, 4, filename);
    return 0;
}