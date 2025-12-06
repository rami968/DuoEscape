#pragma once
#pragma once
#include <string>
#include "Point.h"

using std::string;

class Riddle {
private:
    int riddleID;
    Point position; 
    string question;
    string correctAnswer; 

public:
    Riddle(int id, const Point& pos, const string& q, const string& a)
        : riddleID(id), position(pos), question(q), correctAnswer(a) {
    }

    const string& getQuestion() const { return question; }
    const Point& getPosition() const { return position; }

    bool checkAnswer(const string& playerAnswer) const;
};
