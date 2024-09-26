#pragma once
#include <iostream>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <string>
#include "../../lib/eigen-3.4.0/Eigen/Dense"
#include "Structs.hpp"

// Вывод по центру области
void center_output(std::ofstream& outFile, const std::string& str, int width);


// Создание текстового файла с решением
void solution_txt(Solution& solution, int prs, std::string filename);