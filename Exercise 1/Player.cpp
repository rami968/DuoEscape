#include "Player.h"
#include "Point.h"

Player::Player(const Point& point, const char(&keys)[NUM_KEYS + 1], screen& screen):
	theScreen(screen) {
	p = point;
	memcpy(the_keys, keys, NUM_KEYS * sizeof(the_keys[0]));
}


void Player::handleKeyPressed(char key_pressed) {
	size_t index = 0;
	for (char k : the_keys) {
		//if (std::tolower(k) == 'e' || std::tolower(k) == 'o') {
		//	disposeElement;
		//	return;
		//}
		if (std::tolower(k) == std::tolower(key_pressed)) {
			p.setDirection((Direction)index);
			return;
		}
		++index;
	}
}

void Player::draw() {
	p.draw();
}

void Player::move() {
	p.draw(' ');
	Point p_orig = p;
	p.move();
	if (theScreen.isWall(p)) {
		p = p_orig;
	}
	else if (theScreen.isDoor(p)) {
		p = p_orig;
	}
	else if (theScreen.isSwitchOff(p) || theScreen.isSwitchOn(p)) {
		p = p_orig;
	}
	else if (theScreen.isKey(p)) {
		p = p_orig;
	}
	else if (theScreen.isRiddle(p)) {
		p = p_orig;
	}
	p.draw();
}


// disposeElement();