#pragma once
// STL
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
// Eigen
#include "Dense"
// OpenXLSX
#include "OpenXLSX.hpp"
#include "XLCellReference.hpp"

/*Запись логов*/
class logger
{
    public:
        // Вывод требуемого сообщения
        static void log(const std::string& message, bool to_console = true, const std::string& filename = "")
        {   
            /*Вывод сообщения в консоль*/
            if (to_console) {std::cout << message << std::endl;}

            /*Вывод сообщения в файл*/
            if (!filename.empty()) 
            {
                std::ofstream file(filename, std::ios::app);
                if (file.is_open()) 
                {
                    file << message << std::endl;
                    file.close();
                } 
                else 
                {
                    std::cerr << "Error: unable to open log file.\n";
                }
            }
        }
};



// Вывод столбцов и заголовков у ним в файл .xlsx
template <typename DataV>
void Save_xlsx(const std::vector<std::pair<std::string, DataV>>& Data)
{
    // Ввод имени файла
    std::string filename;
    std::cout << "Enter the name of output file:\n";
    std::getline(std::cin, filename);
    // Ensure .xlsx extension
    if (filename.find(".xlsx") == std::string::npos) 
    {
        filename += ".xlsx";
    }


    using namespace OpenXLSX;

    // Создание файла
    XLDocument doc;
    doc.create(filename, XLForceOverwrite);
    auto wks = doc.workbook().worksheet("Sheet1");
    
    // Запись данных
    int row; // Строка первых элементов векторов (на первой строке заголовки)
    int col = 1; // Столбец (1 соответствует ячейке A и т.д.)
    for (const auto& column : Data) 
    {   
        row = 2;
        // Определение столбца
        const std::string& name = column.first; // Имя столбца
        const auto& vector = column.second; // Вектор данных

        // Создание заголовка
        wks.cell(XLCellReference(1, col)).value() = name;
        //wks.cell(XLCellReference(1, col)).style().setFontBold(true);
        //wks.cell(XLCellReference(1, col)).style().setAlignment(OpenXLSX::Alignment::Center, OpenXLSX::Alignment::Center);
        
        // Вывод вектора данных
        for (const auto& number : vector)
        {
            wks.cell(XLCellReference(row, col)).value() = number;
            //wks.cell(XLCellReference(row, col)).style().setAlignment(OpenXLSX::Alignment::Center, OpenXLSX::Alignment::Center);
            ++row;
        }

        ++col;
    }

    // Закрытие и сохранение
    doc.save();
    doc.close();
    std::cout << "Data successfully saved to " << filename << std::endl;
}

