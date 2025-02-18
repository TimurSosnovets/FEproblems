#pragma once
// Current project
#include "TFE_model.hpp"

// Спускаемый аппарат
class DM
{
    private:
        G_model _gm; // Геометрическая модель (обводы и габариты)
        Lr_model _lrm; // Слоистая модель (информация о слоях)
        TFE_model _fem; // Тепловая КЭ модель

    public:
}
