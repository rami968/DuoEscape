#pragma once
#ifndef COLOR_MANAGER_H
#define COLOR_MANAGER_H

#include "Console.h"

class ColorManager {
public:
    enum Color {
        DEFAULT = 7,   
        GREY = 8,   
        BLUE = 9,   
        GREEN = 10,  
        CYAN = 11,  
        RED = 12,  
        MAGENTA = 13,  
        YELLOW = 14   
    };

    static void setTextColor(Color color, bool isColorEnabled) {
        DuoEscape::Console::setTextColor(static_cast<DuoEscape::Color>(color), isColorEnabled);
    }
};

#endif