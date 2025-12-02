#include "Player.h"
#include "Point.h"
#include <cctype>
#include <cstring>
#include <map>
#include "Doors.h"

Player::Player(const Point& point, const char(&keys)[NUM_KEYS + 1], screen& screen) :
	theScreen(screen) {
	p = point;
	std::memcpy(the_keys, keys, NUM_KEYS * sizeof(the_keys[0]));
	heldElement = ' ';
}

void Player::handleKeyPressed(char key_pressed) {
	char lk = static_cast<char>(std::tolower(static_cast<unsigned char>(key_pressed)));
	if (lk == 'e' || lk == 'o') {
		disposeElement();
		return;
	}

	for (size_t index = 0; index < NUM_KEYS; ++index) {
		char k = the_keys[index];
		if (static_cast<char>(std::tolower(static_cast<unsigned char>(k))) == lk) {
			p.setDirection(Direction(index));
			return;
		}
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
		char targetChar = theScreen.getCharAt(p);
		Doors* currentDoor = theScreen.getDoorByChar(targetChar);
		if (currentDoor != nullptr) {

				// teleport player to the door's destination
		   p = currentDoor->getDestinationPosition();
		   currDoor = currentDoor;
		}
		else {
		   p = p_orig;
		}
	}
	
	else if (theScreen.isSwitchOff(p) || theScreen.isSwitchOn(p)) {
		p = p_orig;
	}
	else if (theScreen.isKey(p)) {
		char elemChar = theScreen.getCharAt(p);
		pickUpElement(elemChar, p);
		theScreen.setCharAt(p, ' ');
	
		theScreen.draw();
		p.draw();
	}
	else if (theScreen.isRiddle(p)) {
		p = p_orig;
	}
	p.draw();
}

void Player::setPosition(const Point& newPos) {
	p = newPos;
}

char Player::getHeldElement() const { return heldElement; }
Point Player::getHeldElementPos() const { return heldElementPos; }
bool Player::hasElement() const { return heldElement != ' '; }
bool Player::hasKey() const { return heldElement == 'K'; }

void Player::pickUpElement(char element, const Point& pos)
{
	heldElement = element;
	heldElementPos = pos;
}


void Player::disposeElement() {
	if (heldElement == ' ')
		return;
	Point elementDropPos = p;
	elementDropPos.move();

	if (theScreen.isWall(elementDropPos) ||
		theScreen.isDoor(elementDropPos) ||
		theScreen.isSwitchOff(elementDropPos) ||
		theScreen.isSwitchOn(elementDropPos) ||
		theScreen.isRiddle(elementDropPos)) {
		return;
	}
	theScreen.setCharAt(elementDropPos, heldElement);
	heldElement = ' ';
};