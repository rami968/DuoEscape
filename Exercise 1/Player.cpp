#include "Player.h"

Player::Player(const point& point, const char(&keys)[NUM_KEYS + 1]){
	auto& p = point;
	memcpy(the_keys, keys, NUM_KEYS * sizeof(the_keys[0]));
}


void Player::handleKeyPressed(char key_pressed) {
	size_t index = 0;
	for (char k : the_keys) {
		if (std::tolower(k) == 'e' || std::tolower(k) == 'o') {
			disposeElement;
			return;
		}
		if (std::tolower(k) == std::tolower(key_pressed)) {
			p.setDirection((Direction)index);
			return;
		}
		++index;
	}
}

void disposeElement();