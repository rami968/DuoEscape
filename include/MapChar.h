#pragma once

enum class MapChar : char {
    Empty = ' ',
    Key = 'K',
    Torch = '!',
    SwitchOff = '\\',
    SwitchOn = '/',
    BombItem = '@',
    Wall = 'W',
    SoftWall = 'w',
    Spring = '#',
    Obstacle = '*',
    Riddle = '?',
    Star = '^',
    Legend = 'L',
    NoItem = '-',
    Player1 = '$',
    Player2 = '&'
};
