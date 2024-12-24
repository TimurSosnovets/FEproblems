// #include "../../headers/thermal/2d_plate.hpp"
// #include <numeric>


// // Слои
// enum Layer {GLASSY_CARBON, TZMK, AMG}; // Итерации по слоям

// // Разбиение на узлы
// void LPlate::nodes_creation(const int W, const std::array<int, 3> L_h)
// {
//     _Nodes.reserve(_DOF);
//     int k = 1; // Номер узла
//     double x, y; // Координаты точки

//     const double step_W = Wdth / W; // Шаг по ширине
//     std::array<double, 3> step_layer; // Шаг по слоям
//     //std::cout << "\nSteps: width - " << step_W << ", layers - ";
//     for (int l = 0; l < 3; ++l)
//     {
//         if (Hght[l] == 0) {step_layer[l] = 0.0;}
//         else {step_layer[l] = Hght[l] / L_h[l];}
//         std::cout << step_layer[l] << " ";
//     }
//     //std::cout << std::endl << "DOF = " << (std::accumulate(L_h.begin(), L_h.end(), 0) + 1) << " * " << W + 1 << std::endl;

//     for (int i_W = 0; i_W < W + 1; ++i_W)
//     {   
//         x = i_W * step_W;
//         Layer CurrentL = GLASSY_CARBON;
//         int i_h = 0;

//         while (i_h < std::accumulate(L_h.begin(), L_h.end(), 0) + 1)
//         {
//             switch (CurrentL) 
//             {                
//                 case GLASSY_CARBON:
//                     if (L_h[0] == 0) {CurrentL = TZMK; continue;}
//                     y = i_h * step_layer[0];
//                     if (i_h == L_h[0]) {CurrentL = TZMK;}
//                 break;

//                 case TZMK:
//                     if (L_h[1] == 0) {CurrentL = AMG; continue;}
//                     y = Hght[0] + (i_h - L_h[0]) * step_layer[1];
//                     if (i_h == L_h[0] + L_h[1]) {CurrentL = AMG;}
//                 break;

//                 case AMG:
//                     if (L_h[2] == 0) {break;}
//                     y = Hght[0] + Hght[1] + (i_h - L_h[0] - L_h[1]) * step_layer[2];
//                 break;
//             }  
//             _Nodes.emplace_back() = {Point(x, y), k};
//             ++k;
//             ++i_h;
//         }
//         i_h = 0;
//     }
//     std::cout << "\n Number of nodes: " << k - 1;
// };

// // Создание элементов 


// // Конструктор класса
// LPlate::LPlate(const std::array<Material, 3> Layer_materials, const double Width, const std::array<double, 3> Layer_hights, const int FEC_width, const std::array<int, 3> FEC_layer)
// : Wdth(Width), Hght(Layer_hights), Mtrl(Layer_materials), _DOF((FEC_width + 1) * (std::accumulate(FEC_layer.begin(), FEC_layer.end(), 0) + 1)) 
// {
//     nodes_creation(FEC_width, FEC_layer);
//     // Тестовый вывод
//     for (const auto& node : _Nodes)
//     {
//         std::cout << "\n Node " << node.second << ": x = " << node.first.x << ", y = " << node.first.y << ".";
//     }
//     std::cout << std::endl;
// };
