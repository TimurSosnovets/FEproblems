#include "../../headers/thermal/LQuad_IP.hpp"

// Узлы
Node n1(Point(0,0), 1), n2(Point(800,0), 2), n3(Point(800,8), 3), n4(Point(0,8), 4);

const std::vector<Node*> Vertices = {&n1, &n2, &n3, &n4};
Eigen::Vector4d Nodal_temps = {300, 400, 500, 600};

int main() 
{
    LQuad Element(Vertices, AMg_6);

    auto GCM = Element.Cond_Mat(Nodal_temps);
    auto GDM = Element.Damp_Mat(Nodal_temps);
    auto T_elem = Element.Element_Temp(Nodal_temps);
    auto F1 = Element.Heat_Load_Surf(1e5, 0, Nodal_temps, {0, 1});
    auto F2 = Element.Heat_Load_Surf(1e5, 0.9, Nodal_temps, {0, 1});

    std::cout << "\n\nElement temperature: " << T_elem << "\n\nConductivity matrix:\n" << GCM << "\n\nDamping matrix\n" << GDM << "\n\nLoad 1:\n" << F1 << "\n\nLoad 2\n" << F2 << std::endl;
    return 0;
};