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
	// Do not accept input while waiting for screen transition
	if (awaitingScreenTransition) {
		return;
	}
	char lk = std::tolower(key_pressed);

	// Last key in the array is the "dispose element" key
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

			// Track the last movement direction (for drop logic when STAY)
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
	// Block movement during screen transition
	if (awaitingScreenTransition) {
		return;
	}

	// Movement rate control (cooldown between moves)
	if (ticksUntilNextMove > 0) {
		--ticksUntilNextMove;
		return;
	}
	ticksUntilNextMove = MOVE_TICK_INTERVAL;

	char backgroundChar = theScreen->getCharAt(p);
	p.draw(backgroundChar);

	// Save original position to revert illegal moves
	Point p_orig = p;
	p.move();

	if (theScreen->isWall(p)) {
		p = p_orig;
	}
	else if (theScreen->isDoor(p)) {
		// handleDoor encapsulates all door/key/teleport logic
		if (handleDoor(p_orig)) {
			return;
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

		// Player can hold only one element at a time
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

		// Player does not stand on the riddle tile, only triggers it
		p = p_orig;
		if (currentRiddle) {
			setActiveRiddle(currentRiddle);
		}
	}

	if (!awaitingScreenTransition) {
		p.draw();
	}
}

bool Player::handleDoor(const Point& p_orig) {
	char targetChar = theScreen->getCharAt(p);
	Doors* currentDoor = theScreen->getDoorByChar(targetChar);
	if (currentDoor != nullptr) {
		const SwitchBoard& switchBoard = theScreen->getSwitchBoard();

		// Deposit key into door if needed (without passing through)
		if (hasElement() && getHeldElement() == 'K' && currentDoor->getRequiredKeyCount() > 0) {
			if (currentDoor->depositKey(keyFirstPos)) {
				consumeHeldKey();
				keyFirstPos = Point(-1, -1, 0, 0, ' ');
			}
			p = p_orig;
			return true;
		}

		// Door cannot be passed yet (switches / keys not satisfied)
		if (!currentDoor->canPlayerPass(switchBoard)) {
			p = p_orig;
			return false;
		}

		// Teleport player through the door
		currentDoor->openDoor();
		char playerChar = p.getChar();
		p = currentDoor->getDestinationPosition();
		p.setChar(playerChar);
		p.setDirection(Direction::STAY);

		// Mark that a screen transition should occur
		currDoor = currentDoor;
		awaitingScreenTransition = true;
		ticksUntilNextMove = 0;
		return true;
	}
	else {
		// No matching door found for this char – revert
		p = p_orig;
		return false;
	}
}

void Player::setPosition(const Point& newPos) {
	char currentCh = p.getChar();
	p = newPos;
	p.setChar(currentCh);
}

char Player::getHeldElement() const { return heldElement; }
bool Player::hasElement() const { return heldElement != ' '; }
bool Player::hasTorch() const { return heldElement == '!'; }

void Player::pickUpElement(char element, const Point& pos)
{
	heldElement = element;
	heldElementPos = pos;

	// Keys are tracked by their original map position (for doors)
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

	// Base direction on current facing; fall back to lastMoveDir or RIGHT
	Direction dir = p.getDirection();
	if (dir == Direction::STAY) {
		if (lastMoveDir != Direction::STAY) {
			dir = lastMoveDir;
		}
		else {
			dir = Direction::RIGHT;
		}
	}

	// Compute drop position one step ahead
	Point elementDropPos = p;
	elementDropPos.setDirection(dir);
	elementDropPos.move();

	// Block dropping on non-empty or special tiles
	if (theScreen->isWall(elementDropPos) ||
		theScreen->isDoor(elementDropPos) ||
		theScreen->isSwitchOff(elementDropPos) ||
		theScreen->isSwitchOn(elementDropPos) ||
		theScreen->isRiddle(elementDropPos) ||
		theScreen->isTorch(elementDropPos) ||
		theScreen->getCharAt(elementDropPos) != ' ') {
		return;
	}

	// Removing key from inventory if we drop it
	if (heldElement == 'K') {
		removeKeyFromInventory(heldElementPos);
	}

	if (heldElement == '@') {
		bombRequested = true;
		bombRequestPos = elementDropPos;
		theScreen->setCharAt(elementDropPos, '@');
		heldElement = ' ';
		p.setDirection(Direction::STAY);
		lastMoveDir = Direction::STAY;
		theScreen->draw();
		p.draw();
		return;
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

bool Player::tryPopBombRequest(Point& out) {
	if (!bombRequested) return false;
	out = bombRequestPos;
	bombRequested = false;
	return true;
}
