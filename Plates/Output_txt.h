#pragma once
#include <iostream>
#include <iomanip>
#include <fstream>
#include <Eigen/Dense>
#include <string>
#include "PSS_Vectors.h"

// Вывод по центру области
void center_output(std::ofstream& outFile, const std::string& str, int width) {
    int padding = (width - str.length()) / 2;  // Calculate left padding
    if (padding > 0) {
        outFile << std::setw(padding + str.length()) << std::right << str;
    } else {
        outFile << str;  // If the string is too long, just output it
    }
    outFile << std::setw(width - padding - str.length()) << "";  // Right padding (if necessary)
}


// Создание текстового файла с решением
        void solution_txt(std::tuple<Eigen::VectorXd, std::vector<std::pair<Eigen::Vector3d, unsigned int>>, std::vector<std::pair<Stress2d, unsigned int>>>& solution, int prs) {
            // Ввод имени файла
            std::cout << "Enter file's name (with .txt extention):\n";
            std::string filename;
            
            std::ofstream file(filename);

            if (file.is_open()) {
                file << std::scientific << std::setprecision(prs);

                // Вывод узловых перемещений
                std::string Column1 = "Node number", Column2 = "Disp. X", Column3 = "Disp. Y";
                int width1 = 14, width2 = 14, width3 = 14; 

                file << "|"; center_output(file, Column1, width1); file << "|"; center_output(file, Column2, width2); file << "|";  center_output(file, Column3, width3); file << "|\n"; 
                file << std::setw(width1 + width2 + width3 + 4) << std::setfill('-') << "" << std::endl;

                for (int i = 0; i < std::get<0>(solution).size(); i+=2) {
                    std::ostringstream number; // Номер i-го узла
                    number << (i/2 + 1);

                    double* ptr = std::get<0>(solution).data(); // Указатель на ВУОП
                    
                    std::ostringstream Displ_X;
                    Displ_X << std::scientific << std::setprecision(prs) << *(ptr + i);

                    std::ostringstream Displ_Y;
                    Displ_Y << std::scientific << std::setprecision(prs) << *(ptr + i + 1);

                    file << "|"; center_output(file, number.str(), width1); file << "|"; center_output(file, Displ_X.str(), width2); file << "|";  center_output(file, Displ_Y.str(), width3); file << "|";
                
                 // Очищение строк
                    number.str("");        // Clear the buffer
                    number.clear();        // Reset any error flags
                    Displ_X.str("");        
                    Displ_X.clear();
                    Displ_Y.str("");        
                    Displ_Y.clear();                   
                }

                file << std::endl << std::endl << std::endl;

                // Вывод векторов деформаций и напряжений
                std::string Column1 = "Node number", Column2 = "Strain vector (transposed)", Column3 = "Stress vector (transposed)", Column4 = "Von Mises (σ1, σ2)";
                int width1 = 14, width2 = 45, width3 = 45, width4 = 30;

                file << "|"; center_output(file, Column1, width1); file << "|"; center_output(file, Column2, width2); file << "|";  center_output(file, Column3, width3); file << "|"; center_output(file, Column4, width4); file << "|\n";
                file << std::setw(width1 + width2 + width3 + width4 + 5) << std::setfill('-') << "" << std::endl;

                for (int i = 0; i < std::get<1>(solution).size(); ++i) {

                    std::ostringstream number; // Номер i-го узла
                    number << std::get<1>(solution)[i].second;

                    std::ostringstream strain; // Вектор деформаций i-го КЭ                   
                    double* ptr_strain = std::get<1>(solution)[i].first.data(); // Указатель на вектор деформаций i-го КЭ
                    strain << std::scientific << std::setprecision(prs) << "{" << *ptr_strain << ", " << *(ptr_strain + 1) << ", " << *(ptr_strain + 2) << "}";

                    std::ostringstream stress; // Вектор напряжений i-го КЭ
                    double* ptr_stress = std::get<2>(solution)[i].first.coord().data(); // Указатель на вектор напряжений i-го КЭ
                    stress << std::scientific << std::setprecision(prs) << "{" << *ptr_stress << ", " << *(ptr_stress + 1) << ", " << *(ptr_stress + 2) << "}";

                    std::ostringstream Von_Mises; // Эквивалентное напряжение в i-м КЭ
                    std::pair<double, double>* ptr_von = new std::pair<double, double>;
                    *ptr_von = std::get<2>(solution)[i].first.sigma_principal();
                    Von_Mises << std::scientific << std::setprecision(prs) << "(" << ptr_von->first << ", " << ptr_von->second << ")";
                    delete ptr_von;

                    // Непосредственно строка
                    file << "|"; center_output(file, number.str(), width1); file << "|"; center_output(file, strain.str(), width2); file << "|";  center_output(file, stress.str(), width3); file << "|"; center_output(file, Von_Mises.str(), width4); file << "|\n";                                    

                    // Очищение строк
                    number.str("");        // Clear the buffer
                    number.clear();        // Reset any error flags
                    strain.str("");        
                    strain.clear();
                    stress.str("");        
                    stress.clear();
                    Von_Mises.str("");        
                    Von_Mises.clear();
                }
                file.close();
                std::cout << "Data written to " << filename << " successfully." << std::endl;
            } else {
                std::cerr << "Unable to open file!" << std::endl;
            }
            
        }