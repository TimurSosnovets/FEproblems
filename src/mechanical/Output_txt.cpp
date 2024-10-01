#include "../../headers/mechanical/Output_txt.hpp"

// Вывод по центру области
void center_output(std::ofstream& outFile, const std::string& str, int width) {
    int padding = (width - str.length()) / 2;  // Calculate left padding
    if (padding > 0) {
        outFile << std::setw(padding + str.length()) << std::right << str;
    } else {
        outFile << str;  // If the string is too long, just output it
    }
     
    outFile << std::setw(width - padding - str.length()) << ""; // Right padding (if necessary)      
}


// Создание текстового файла с решением
void solution_txt(Solution& solution, int prs, std::string filename) {
    std::string directory = "./Results/";
    std::string full_path = directory + filename;
    std::filesystem::path dirPath(directory);
    if (!std::filesystem::exists(dirPath)) {
        std::filesystem::create_directories(dirPath);  
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
        std::vector<std::string> Headline(5);
        Headline[0] = "№ el."; Headline[1] = "Disp. X"; Headline[2] = "Disp. Y";
        std::vector<int> Hwidth(4);
        Hwidth[0] = 5; Hwidth[1] = 9 + prs; Hwidth[2] = 9 + prs; 

        file << "|"; center_output(file, Headline[0], Hwidth[0]); // 1 столбец
        file << "|"; center_output(file, Headline[1], Hwidth[1]); // 2 столбец
        file << "|";  center_output(file, Headline[2], Hwidth[2]); // 3 столбец
        file << "|\n"; 
        file << std::setw(Hwidth[0] + Hwidth[1] + Hwidth[2] + 4) << std::setfill('-') << "" << std::endl << std::setfill(' '); // отделение шапки

        for (int i = 0; i < solution.Nd_Dspl.size(); i+=2) {
            std::ostringstream number; // Номер i-го узла
            number << (i/2 + 1);

            double* ptr = solution.Nd_Dspl.data(); // Указатель на ВУОП

            std::ostringstream Displ_X;
            Displ_X << std::scientific << std::setprecision(prs) << *(ptr + i);

            std::ostringstream Displ_Y;
            Displ_Y << std::scientific << std::setprecision(prs) << *(ptr + i + 1);

            file << "|"; center_output(file, number.str(), Hwidth[0]); // 1 стоблец
            file << "|"; center_output(file, Displ_X.str(), Hwidth[1]); // 2 столбец
            file << "|";  center_output(file, Displ_Y.str(), Hwidth[2]); // 3 столбец
            file << "|\n";

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
        Headline[0] = "№ FE"; Headline[1] = "Strain vector (transposed)"; Headline[2] = "Stress vector (transposed)"; Headline[3] = "Principal stresses"; Headline[4] = "Von Mises";
        Hwidth[0] = 4; Hwidth[1] = 42 + prs; Hwidth[2] = 43 + prs; Hwidth[3] = 27 + prs; Hwidth[4] = 8 + prs;

        file << "|"; center_output(file, Headline[0], Hwidth[0]); // 1 столбец
        file << "|"; center_output(file, Headline[1], Hwidth[1]); // 2 столбец
        file << "|";  center_output(file, Headline[2], Hwidth[2]); // 3 столбец
        file << "|"; center_output(file, Headline[3], Hwidth[3]); // 4 столбец
        file << "|"; center_output(file, Headline[4], Hwidth[4]); // 5 столбец
        file << "|\n";
        file << std::setw(Hwidth[0] + Hwidth[1] + Hwidth[2] + Hwidth[3] + Hwidth[4] + 6) << std::setfill('-') << "" << std::endl << std::setfill(' '); // отделение шапки

        for (int i = 0; i < solution.FE_Strains.size(); ++i) {

            std::ostringstream number; // Номер i-го узла
            number << solution.FE_Strains[i].second;

            std::ostringstream strain; // Вектор деформаций i-го КЭ                   
            double* ptr_strain = solution.FE_Strains[i].first.data(); // Указатель на вектор деформаций i-го КЭ
            strain << std::scientific << std::setprecision(prs) << "{" << *ptr_strain << ", " << *(ptr_strain + 1) << ", " << *(ptr_strain + 2) << "}";

            std::ostringstream stress; // Вектор напряжений i-го КЭ
            
            stress << std::scientific << std::setprecision(prs) << "{" << solution.FE_Stresses[i].first.sigma_x() << ", " << solution.FE_Stresses[i].first.sigma_y() << ", " << solution.FE_Stresses[i].first.tau_xy() << "}";

            std::ostringstream Princ_Stress; // Главные напряжения в i-м КЭ
            std::pair<double, double>* ptr_princ = new std::pair<double, double>;
            *ptr_princ = solution.FE_Stresses[i].first.sigma_principal();
            Princ_Stress << std::scientific << std::setprecision(prs) << "(" << ptr_princ->first << ", " << ptr_princ->second << ")";
            delete ptr_princ;

            std::ostringstream Eq_Stress; // Эквивалентное напряжение в i-м КЭ
            Eq_Stress << std::scientific << std::setprecision(prs) << solution.FE_Stresses[i].first.equivalent_stress();

            // Непосредственно строка
            file << "|"; center_output(file, number.str(), Hwidth[0]); // 1 столбец
            file << "|"; center_output(file, strain.str(), Hwidth[1]); // 2 столбец
            file << "|";  center_output(file, stress.str(), Hwidth[2]); // 3 столбец
            file << "|"; center_output(file, Princ_Stress.str(), Hwidth[3]); // 4 столбец
            file << "|"; center_output(file, Eq_Stress.str(), Hwidth[4]); // 5 столбец
            file << "|\n";                                    

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