#include "Player.h"
#include "Point.h"
#include <cctype>
#include <cstring>
#include "Doors.h"

Player::Player(const Point& point, const char(&keys)[NUM_KEYS + 1], screen& screen) :
	theScreen(&screen), p(point) {
	std::memcpy(the_keys, keys, NUM_KEYS * sizeof(the_keys[0]));
}

void Player::handleKeyPressed(char key_pressed) {
	if (awaitingScreenTransition) {
		return;
	}
	char lk = std::tolower(key_pressed);
	char disposeKey = the_keys[NUM_KEYS - 1];
	if (lk == disposeKey) {
		disposeElement();
		return;
	}

	for (size_t index = 0; index < NUM_KEYS; ++index) {
		char k = the_keys[index];
		if (k == lk) {
			Direction dir = static_cast<Direction>(index);
			p.setDirection(dir);
			if (dir != Direction::STAY) {
				lastMoveDir = dir;
			}
			return;

		}
	}
}

void Player::consumeHeldKey() {
	if (heldElement == 'K') {
		removeKeyFromInventory(keyFirstPos);
		heldElement = ' ';
	}
}

void Player::draw() {
	if (awaitingScreenTransition) {
		return;
	}
	p.draw();
}

void Player::move() {
	if (awaitingScreenTransition) {
		return;
	}
	// function by Copilot 
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
			const SwitchBoard& switchBoard = theScreen->getSwitchBoard();
			if (hasElement() && getHeldElement() == 'K' && currentDoor->getRequiredKeyCount() > 0) {
				if (currentDoor->depositKey(keyFirstPos)) {
					consumeHeldKey();
					keyFirstPos = Point(-1, -1, 0, 0, ' ');
				}
				p = p_orig;
				return;
			}
			if (!currentDoor->canPlayerPass(switchBoard)) {
				p = p_orig;
			}
			else { 
				screen prevScreen = *theScreen;
				currentDoor->openDoor();
				char playerChar = p.getChar();
				if (currentDoor->getDestinationScreenID() != theScreen->getCurrentScreenID()) {
					currDoor = currentDoor;
					awaitingScreenTransition = true;
				}
				else {
					p.draw(theScreen->getCharAt(p));
					p = currentDoor->getDestinationPosition();
					theScreen->setCharAt(p, ' ');
					p.setChar(playerChar);
					p.setDirection(Direction::STAY);
					p.draw(); 
				}
				ticksUntilNextMove = 0;
				return;
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
	else if (theScreen->isKey(p) || theScreen->isTorch(p)) {
		char elemChar = theScreen->getCharAt(p);
		if (hasElement()) {
			p = p_orig;
			p.draw();
			return;
		}
		pickUpElement(elemChar, p);
		theScreen->setCharAt(p, ' ');
		theScreen->draw();
		p.draw();
	}
	else if (theScreen->isRiddle(p)) {
		Riddle* currentRiddle = theScreen->getRiddleByPosition(p);
		p = p_orig;
		if (currentRiddle) {
			setActiveRiddle(currentRiddle);
		}

	}
	if (!awaitingScreenTransition) {
		p.draw();
	}
}

void Player::setPosition(const Point& newPos) {
	char currentCh = p.getChar();
	p = newPos;
	p.setChar(currentCh); // keep the player's glyph when teleporting
}

char Player::getHeldElement() const { return heldElement; }
bool Player::hasElement() const { return heldElement != ' '; }
bool Player::hasTorch() const { return heldElement == '!'; }
void Player::pickUpElement(char element, const Point& pos)
{
	heldElement = element;
	heldElementPos = pos;
	if (element == 'K') {
		const Point* originalID = theScreen->findOriginalKeyID(pos);

		if (originalID) {
			keyFirstPos = *originalID;

			if (!hasKeyInInventory(*originalID)) {
				collectedKeys.push_back(*originalID);
			}

		}
	}
}

Point Player::getHeldElementPos() const {
	if (heldElement == 'K') {
		return keyFirstPos;
	}
	return heldElementPos;
}

void Player::resetTransitionSignal() {
	currDoor = nullptr;
	awaitingScreenTransition = false;
	ticksUntilNextMove = 0;
}


void Player::disposeElement() {
	if (heldElement == ' ')
		return;
	 Direction dir = p.getDirection();

    if (dir == Direction::STAY) {
        if (lastMoveDir != Direction::STAY) {
            dir = lastMoveDir;
        }
        else {
            dir = Direction::RIGHT;
        }
    }

    Point elementDropPos = p;
    elementDropPos.setDirection(dir); 
    elementDropPos.move();          


	if (theScreen->isWall(elementDropPos) ||
		theScreen->isDoor(elementDropPos) ||
		theScreen->isSwitchOff(elementDropPos) ||
		theScreen->isSwitchOn(elementDropPos) ||
		theScreen->isRiddle(elementDropPos) ||
	    theScreen->isTorch(elementDropPos)     ||
        theScreen->getCharAt(elementDropPos) != ' ') {
		return;
	}
	
	if (heldElement == 'K') {
		removeKeyFromInventory(heldElementPos);
	}

	theScreen->setCharAt(elementDropPos, heldElement);
	heldElement = ' ';
	p.setDirection(Direction::STAY);
    lastMoveDir = Direction::STAY;
	theScreen->draw();
	p.draw();
}

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