#pragma once
#include "Triangle_2dof.hpp"
#include "Structs.hpp"

class Plate_triangulated;
// Разбиение на узлы
void Nodes_creation(std::vector<std::pair<Point, unsigned int>>& Nodes, const int DoF, const double a, const double b, const int m, const int n);

// Ассамблирование
template <typename T>
typename std::enable_if<std::is_same<T, TriangleFE>::value, void>::type
Assembly (Eigen::MatrixXd& GSM, const int& DoF, const std::vector<std::pair<T, unsigned int>>& FEs);

// Получение узловых перемещений
template <typename T>
typename std::enable_if<std::is_same<T, Plate_triangulated>::value, Eigen::VectorXd>::type
Ndl_Dsplcmnts(const T& Plate, LBC LBC);

//Вектор деформаций в каждой точке треугольного КЭ при ПНС
template <typename T>
typename std::enable_if<std::is_same<T, TriangleFE>::value, Eigen::Vector3d>::type 
Strain(const Eigen::VectorXd& Vertice_displacements, const std::pair<T, unsigned int>& FE);

//Вектор напряжений в каждой точке треугольного КЭ при ПНС
template <typename T>
typename std::enable_if<std::is_same<T, TriangleFE>::value, Stress2d>::type 
Stress(const Eigen::VectorXd& Vertice_displacements, const std::pair<T, unsigned int>& FE);

// Полное решение задачи (Узловые перемещения, векторы деформаций и напряжений для всех КЭ при ПНС)
template <typename T>
typename std::enable_if<std::is_same<T, Plate_triangulated>::value, Solution>::type
solve(T& Plate, const LBC LBC);

