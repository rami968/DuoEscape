#pragma once

#include <string>

namespace DuoEscape {

enum class Color {
    DEFAULT = 7,   
    GREY = 8,   
    BLUE = 9,   
    GREEN = 10,  
    CYAN = 11,  
    RED = 12,  
    MAGENTA = 13,  
    YELLOW = 14   
};

class Console {
public:
    static void gotoXY(int x, int y);
    static void hideCursor();
    static void clearScreen();
    static void setTextColor(Color color, bool isColorEnabled = true);
};

} // namespace DuoEscape
