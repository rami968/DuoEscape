#pragma once
#pragma once
#include <string>
#include <vector>
#include "Point.h"

using std::string;

class Riddle {

private:
    int riddleID;
    Point position;
    string question;
    std::vector<string> correctAnswers;

public:
    Riddle(int id, const Point& pos, const string& q, const std::vector<string>& answers)
        : riddleID(id), position(pos), question(q), correctAnswers(answers) {
    }
    const string& getQuestion() const { return question; }
    const Point& getPosition() const { return position; }
    const std::vector<string>& getCorrectAnswers() const { return correctAnswers; }
    bool checkAnswer(const string& playerAnswer) const;
};
