#include "../../headers/thermal/LQuad_IP_FE.hpp"

int main() 
{
    std::pair<Point, int> v1 {Point(-400, -4), 1};
    std::pair<Point, int> v2 {Point(400, -4), 2};
    std::pair<Point, int> v3 {Point(400, 4), 3};
    std::pair<Point, int> v4 {Point(-400, 4), 4};
    
    std::array<std::reference_wrapper<std::pair<Point, int>>, 4> Vertices = {std::ref(v1), std::ref(v2), std::ref(v3), std::ref(v4)};
    double lambda = 50.8;
    LQuad Plate = LQuad(Vertices, lambda, lambda);

    std::cout << "Conductivity matrix: \n" << Plate.Cond_Mat() << std::endl;
}