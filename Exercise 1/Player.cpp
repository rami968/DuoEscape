#include "Player.h"
#include "Point.h"
#include <cctype>
#include <cstring>
#include <map>
#include "Doors.h"

Player::Player(const Point& point, const char(&keys)[NUM_KEYS + 1], screen& screen) :
	theScreen(&screen) {
	p = point;
	std::memcpy(the_keys, keys, NUM_KEYS * sizeof(the_keys[0]));
	heldElement = ' ';
}

void Player::handleKeyPressed(char key_pressed) {
	char lk = std::tolower(key_pressed);
	if (lk == 'e' || lk == 'o') {
		disposeElement();
		return;
	}
	for (size_t index = 0; index < NUM_KEYS; ++index) {
		char k = the_keys[index];
		if (k == lk) {
			p.setDirection((Direction)index);
			return;
		}
	}
}

void Player::draw() {
	p.draw();
}

void Player::move() {
	// function by copylot 
	if (ticksUntilNextMove > 0) {
		--ticksUntilNextMove;
		return; 
	}
	ticksUntilNextMove = MOVE_TICK_INTERVAL;
	char backgroundChar = theScreen->getCharAt(p);
	p.draw(backgroundChar);
	Point p_orig = p;
	p.move();
	if (theScreen->isWall(p)) {
		p = p_orig;
	}
	else if (theScreen->isDoor(p)) {
		char targetChar = theScreen->getCharAt(p);
		Doors* currentDoor = theScreen->getDoorByChar(targetChar);
		if (currentDoor != nullptr) {
			const auto& switchStates = theScreen->getSwitchStates();
			if (!currentDoor->canPlayerPass(collectedKeys, switchStates)) {
				p = p_orig;
			}
			else {
				currentDoor->openDoor(collectedKeys);
				if (heldElement == 'K' && !hasKeyInInventory(heldElementPos)) {
					heldElement = ' ';
				}
				char playerChar = p.getChar();
				// teleport player to the door's destination but keep its glyph
	   p = currentDoor->getDestinationPosition();
	   p.setChar(playerChar);
	   p.setDirection(Direction::STAY);
	   currDoor = currentDoor;
			}
		}
		else {
		   p = p_orig;
		}
	}
	
	else if (theScreen->isSwitchOff(p) || theScreen->isSwitchOn(p)) {
		bool steppedOntoSwitch = (p.getX() != p_orig.getX()) || (p.getY() != p_orig.getY());
		if (steppedOntoSwitch) {
			theScreen->toggleSwitchAt(p);
		}
	}
	else if (theScreen->isKey(p)) {
		char elemChar = theScreen->getCharAt(p);
		pickUpElement(elemChar, p);
		theScreen->setCharAt(p, ' ');
	
		theScreen->draw();
		p.draw();
	}
	else if (theScreen->isRiddle(p)) {
		p = p_orig;
	}
	p.draw();
}

void Player::setPosition(const Point& newPos) {
	char currentCh = p.getChar();
	p = newPos;
	p.setChar(currentCh); // keep the player's glyph when teleporting
}

char Player::getHeldElement() const { return heldElement; }
Point Player::getHeldElementPos() const { return heldElementPos; }
bool Player::hasElement() const { return heldElement != ' '; }
void Player::pickUpElement(char element, const Point& pos)
{
	heldElement = element;
	heldElementPos = pos;
	if (element == 'K') {
		collectedKeys.push_back(pos);
	}
}


void Player::disposeElement() {
	if (heldElement == ' ')
		return;
	Point elementDropPos = p;
	elementDropPos.move();

	if (theScreen->isWall(elementDropPos) ||
		theScreen->isDoor(elementDropPos) ||
		theScreen->isSwitchOff(elementDropPos) ||
		theScreen->isSwitchOn(elementDropPos) ||
		theScreen->isRiddle(elementDropPos)) {
		return;
	}
	theScreen->setCharAt(elementDropPos, heldElement);
	if (heldElement == 'K') {
		removeKeyFromInventory(heldElementPos);
	}
	heldElement = ' ';
};

bool Player::hasKeyInInventory(const Point& keyPos) const {
	for (const auto& storedKey : collectedKeys) {
		if (storedKey.getX() == keyPos.getX() && storedKey.getY() == keyPos.getY()) {
			return true;
		}
	}
	return false;
}

bool Player::removeKeyFromInventory(const Point& keyPos) {
	for (auto it = collectedKeys.begin(); it != collectedKeys.end(); ++it) {
		if (it->getX() == keyPos.getX() && it->getY() == keyPos.getY()) {
			collectedKeys.erase(it);
			return true;
		}
	}
	return false;
}