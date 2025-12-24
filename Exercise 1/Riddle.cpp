#include "Riddle.h"
#include <algorithm>
#include <cctype>
#include <string>

using std::string;

// Converts a string to lowercase (helper for case-insensitive comparison).
std::string toLower(const std::string& str) {
    std::string lower_str = str;
    std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return lower_str;
}

// Compares player's answer to the correct answer, ignoring letter case.
bool Riddle::checkAnswer(const std::string& playerAnswer) const {
    std::string normalizedPlayerAnswer = toLower(playerAnswer);
    std::string normalizedCorrectAnswer = toLower(correctAnswer);

    return normalizedPlayerAnswer == normalizedCorrectAnswer;
}
