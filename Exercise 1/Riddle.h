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
    // Creates a new riddle with id, position, question and correct answer.
    Riddle(int id, const Point& pos, const string& q, const string& a)
        : riddleID(id), position(pos), question(q), correctAnswer(a) {
    }

    // Returns the question text.
    const string& getQuestion() const { return question; }

    // Returns the map position of this riddle.
    const Point& getPosition() const { return position; }

    // Checks whether the player's answer is equal to the correct answer.
    bool checkAnswer(const string& playerAnswer) const;
};
