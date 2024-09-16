#pragma once
#include <iostream>
#include <iomanip>
#include <fstream>
#include <Eigen/Dense>
#include <string>
#include "PSS_Vectors.h"

// Создание текстового файла с решением
        void solution_txt(const std::tuple<Eigen::VectorXd, std::vector<std::pair<Eigen::Vector3d, unsigned int>>, std::vector<std::pair<Stress2d, unsigned int>>>& solution) {
            // Ввод имени файла
            std::cout << "Enter file's name (with .txt extention):\n";
            std::string filename;
            
            std::ofstream file(filename);

            if (file.is_open()) {
                file << std::scientific << std::setprecision(5);

                // Вывод узловых перемещений               
                file << "| Node nuber |    Disp. X    |    Disp. Y   |\n";
                for (int i = 0; i < std::get<0>(solution).size(); i+=2) {
                    // Номер узла
                    if (i < 18) {file << "|      " << (i/2 + 1) << "     | ";}
                    if ((i > 17) && (i < 198)) {file << "|     " << (i/2 + 1) << "     | ";}
                    else {file << "|    " << (i/2 + 1) << "     | ";}
                    // Перемещение вдоль оси Х
                    if (std::get<0>(solution)[i] > 0) {
                        file << " " << std::get<0>(solution)[i] << "  | ";
                    } else { file << std::get<0>(solution)[i] << "  | ";}
                    // Перемещение вдоль оси У
                    if (std::get<0>(solution)[i + 1] > 0) {
                        file << " " << std::get<0>(solution)[i + 1] << "  |";
                    } else {file << std::get<0>(solution)[i + 1] << "  |";}
                    file << std::endl;                     
                }

                // Вывод векторов деформаций и напряжений
                

                file.close();
                std::cout << "Data written to " << filename << " successfully." << std::endl;
            } else {
                std::cerr << "Unable to open file!" << std::endl;
            }
            
        }