#pragma once
#include <iostream>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <string>
#include "../toolbox/eigen-3.4.0/Eigen/Dense"
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
        void solution_txt(Solution& solution, int prs, std::string filename) {
                        
            std::string directory = "./Results/";

            // Create the full path by concatenating the directory and the user-provided filename
            std::string full_path = directory + filename;

            // Create the directory if it doesn't exist
            std::filesystem::path dirPath(directory);
            if (!std::filesystem::exists(dirPath)) {
                std::filesystem::create_directories(dirPath);  // Ensure the directory exists
            }
            
            
            std::ofstream file(full_path);
            if (file.is_open()) {
             std::cout << std::endl << "File " << filename << " opened successfully." << std::endl;
            } else {
             std::cerr << "Unable to open file " << filename << "!" << std::endl << std::endl;
            }

            if (file.is_open()) {
                file << std::scientific << std::setprecision(prs);

                // Вывод узловых перемещений
                std::vector<std::string> Headline(4);
                Headline[0] = "Node number"; Headline[1] = "Disp. X"; Headline[2] = "Disp. Y";
                std::vector<int> Hwidth(4);
                Hwidth[0] = 13; Hwidth[1] = 14; Hwidth[2] = 14; 

                file << "|"; center_output(file, Headline[0], Hwidth[0]); file << "|"; center_output(file, Headline[1], Hwidth[1]); file << "|";  center_output(file, Headline[2], Hwidth[2]); file << "|\n"; 
                file << std::setw(Hwidth[0] + Hwidth[1] + Hwidth[2] + 4) << std::setfill('-') << "" << std::endl << std::setfill(' ');

                for (int i = 0; i < solution.Nd_Dspl.size(); i+=2) {
                    std::ostringstream number; // Номер i-го узла
                    number << (i/2 + 1);

                    double* ptr = solution.Nd_Dspl.data(); // Указатель на ВУОП
                    
                    std::ostringstream Displ_X;
                    Displ_X << std::scientific << std::setprecision(prs) << *(ptr + i);

                    std::ostringstream Displ_Y;
                    Displ_Y << std::scientific << std::setprecision(prs) << *(ptr + i + 1);

                    file << "|"; center_output(file, number.str(), Hwidth[0]); file << "|"; center_output(file, Displ_X.str(), Hwidth[1]); file << "|";  center_output(file, Displ_Y.str(), Hwidth[2]); file << "|\n";
                
                 // Очищение строк
                    number.str("");        // Clear the buffer
                    number.clear();        // Reset any error flags
                    Displ_X.str("");        
                    Displ_X.clear();
                    Displ_Y.str("");        
                    Displ_Y.clear();                   
                }
                Headline.clear();
                Hwidth.clear();

                file << std::endl << std::endl << std::endl;


                // Вывод векторов деформаций и напряжений
                
                Headline[0] = "FE number"; Headline[1] = "Strain vector (transposed)"; Headline[2] = "Stress vector (transposed)"; Headline[3] = "Principal stresses";
                Hwidth[0] = 11; Hwidth[1] = 45; Hwidth[2] = 45; Hwidth[3] = 30;

                file << "|"; center_output(file, Headline[0], Hwidth[0]); file << "|"; center_output(file, Headline[1], Hwidth[1]); file << "|";  center_output(file, Headline[2], Hwidth[2]); file << "|"; center_output(file, Headline[3], Hwidth[3]); file << "|\n";
                file << std::setw(Hwidth[0] + Hwidth[1] + Hwidth[2] + Hwidth[3] + 5) << std::setfill('-') << "" << std::endl << std::setfill(' ');

                for (int i = 0; i < solution.FE_Strains.size(); ++i) {

                    std::ostringstream number; // Номер i-го узла
                    number << solution.FE_Strains[i].second;

                    std::ostringstream strain; // Вектор деформаций i-го КЭ                   
                    double* ptr_strain = solution.FE_Strains[i].first.data(); // Указатель на вектор деформаций i-го КЭ
                    strain << std::scientific << std::setprecision(prs) << "{" << *ptr_strain << ", " << *(ptr_strain + 1) << ", " << *(ptr_strain + 2) << "}";

                    std::ostringstream stress; // Вектор напряжений i-го КЭ
                    double* ptr_stress = solution.FE_Stresses[i].first.coord().data(); // Указатель на вектор напряжений i-го КЭ
                    stress << std::scientific << std::setprecision(prs) << "{" << *ptr_stress << ", " << *(ptr_stress + 1) << ", " << *(ptr_stress + 2) << "}";

                    std::ostringstream Princ_Stress; // Эквивалентное напряжение в i-м КЭ
                    std::pair<double, double>* ptr_von = new std::pair<double, double>;
                    *ptr_von = solution.FE_Stresses[i].first.sigma_principal();
                    Princ_Stress << std::scientific << std::setprecision(prs) << "(" << ptr_von->first << ", " << ptr_von->second << ")";
                    delete ptr_von;

                    // Непосредственно строка
                    file << "|"; center_output(file, number.str(), Hwidth[0]); file << "|"; center_output(file, strain.str(), Hwidth[1]); file << "|";  center_output(file, stress.str(), Hwidth[2]); file << "|"; center_output(file, Princ_Stress.str(), Hwidth[3]); file << "|\n";                                    

                    // Очищение строк
                    number.str("");        // Clear the buffer
                    number.clear();        // Reset any error flags
                    strain.str("");        
                    strain.clear();
                    stress.str("");        
                    stress.clear();
                    Princ_Stress.str("");        
                    Princ_Stress.clear();
                }
                
                file.close();
                std::cout << "Data written to " << filename << " successfully." << std::endl;
            } else {
                std::cerr << "Unable to open file!" << std::endl;
            }
            
        }