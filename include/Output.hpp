#pragma once
// STL
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>
#include <ctime>
#include <chrono>
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

// Форматирование временных точек
inline std::string format_time(std::chrono::system_clock::time_point tp) {
    std::time_t time = std::chrono::system_clock::to_time_t(tp);
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

// Контейнер результатов
struct Results_transient
{
    // Векторы узловых температур в обозначенные моменты времени
    std::vector<std::pair<float, Eigen::VectorXd>> VNT_samples; 
    // Значение температур некоторых узлов во времени
    std::vector<std::pair<size_t, std::vector<double>>> NT_samples;
    // Конструктор
    Results_transient(std::vector<float>& time_moments, std::vector<size_t>& node_numbers, size_t DOF, size_t time_steps)
    {
        VNT_samples.reserve(time_moments.size());
        NT_samples.resize(node_numbers.size());
        // for (size_t i = 0; i < time_moments.size(); ++i)
        // {
        //     VNT_samples[i].second.resize(DOF);
        // }
        for (size_t i = 0; i < node_numbers.size(); ++i)
        {
            NT_samples[i].first = node_numbers[i];
            NT_samples[i].second.reserve(time_steps);
        }
    }
};

// Вывод столбцов и заголовков у ним в файл .xlsx
template <typename DataV>
void Save_xlsx(const std::vector<std::pair<std::string, DataV>>& Data)
{
    // Ввод имени файла
    std::string filename = "results";
    // std::cout << "Enter the name of output file:\n";
    // std::getline(std::cin, filename);
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

template <typename DataV>
void Save_xlsx(const std::vector<std::pair<double, DataV>>& Data)
{
    // Ввод имени файла
    std::string filename = "results";
    // std::cout << "Enter the name of output file:\n";
    // std::getline(std::cin, filename);
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
        const std::string& name = "Time " + std::to_string(column.first); // Имя столбца
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

template <typename DataV>
void Save_xlsx(std::string filename, const std::vector<std::pair<double, DataV>>& Data)
{
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
        const std::string& name = "Time " + std::to_string(column.first); // Имя столбца
        const auto& vector = column.second; // Вектор данных

        // Создание заголовка
        wks.cell(XLCellReference(1, col)).value() = name;
        
        // Вывод вектора данных
        for (const auto& number : vector)
        {
            wks.cell(XLCellReference(row, col)).value() = number;
            ++row;
        }

        ++col;
    }

    // Закрытие и сохранение
    doc.save();
    doc.close();
    std::cout << "Data successfully saved to " << filename << std::endl;
}

