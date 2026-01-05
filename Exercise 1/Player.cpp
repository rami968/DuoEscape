#include "Player.h"
#include "Point.h"
#include <cctype>
#include <cstring>
#include "Doors.h"

Player::Player(const Point& point, const char(&keys)[NUM_KEYS + 1], screen& screen) :
	theScreen(&screen), p(point) {
	std::memcpy(the_keys, keys, NUM_KEYS * sizeof(the_keys[0]));
}

void Player::handleKeyPressed(char key_pressed) {  // Handle key press for movement and disposal
	if (awaitingScreenTransition) {
		return;
	}
	char lk = std::tolower(key_pressed);
	char disposeKey = the_keys[NUM_KEYS - 1];
	if (lk == disposeKey) {
		disposeElement(); // Handle element disposal
		return;
	}

	for (size_t index = 0; index < NUM_KEYS; ++index) {
		char k = the_keys[index];
		if (k == lk) {
			Direction dir = static_cast<Direction>(index);
			p.setDirection(dir);
			if (dir != Direction::STAY) {
				lastMoveDir = dir; // Remember last moving direction
			}
			return;

		}
	}
}

void Player::consumeHeldKey() {  // Remove held key from inventory
	if (heldElement == 'K') {
		removeKeyFromInventory(keyFirstPos); // Remove key from collected keys vector
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
	// movement timing
	if (ticksUntilNextMove > 0) { // Check if movement interval has passed
		--ticksUntilNextMove;
		return;
	}

	ticksUntilNextMove = MOVE_TICK_INTERVAL;

	char backgroundChar = theScreen->getCharAt(p);
	p.draw(backgroundChar); // Redraw background at current position
	Point p_orig = p;
	p.move(); // Calculate new position
	if (theScreen->isWall(p)) {
		p = p_orig; // Collision: revert position
	}
	else if (theScreen->isDoor(p)) {
		char targetChar = theScreen->getCharAt(p);
		Doors* currentDoor = theScreen->getDoorByChar(targetChar);
		handleDoorInteraction(p_orig, targetChar, currentDoor); // Handle door logic (pass/deposit/block)
	}
	else if (theScreen->getCharAt(p) == '@' && theScreen->isBombArmedAt(p)) {
		p = p_orig;  // active bomb on the ground(counting down to explode) – block stepping onto this tile
	}
	else if (theScreen->isSwitchOff(p) || theScreen->isSwitchOn(p)) {
		bool steppedOntoSwitch = (p.getX() != p_orig.getX()) || (p.getY() != p_orig.getY());
		if (steppedOntoSwitch) {
			theScreen->toggleSwitchAt(p); // Toggle switch state
		}
	}
	else if (theScreen->isKey(p) || theScreen->isTorch(p) || theScreen->isBomb(p)) {
		char elemChar = theScreen->getCharAt(p);
		if (hasElement()) {
			p = p_orig; // Cannot pick up if inventory is full
			p.draw();
			return;
		}
		pickUpElement(elemChar, p); // Update inventory
		theScreen->setCharAt(p, ' '); // Remove element from map
		theScreen->draw();
		p.draw();
	}
	else if (theScreen->isRiddle(p)) {
		Riddle* currentRiddle = theScreen->getRiddleByPosition(p);
		p = p_orig; // Stop on riddle, movement handled by control unit
		if (currentRiddle) {
			setActiveRiddle(currentRiddle); // Signal riddle solving sequence
		}
	}
	if (!awaitingScreenTransition) {
		p.draw(); // Draw player at final position
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
		const Point* originalID = theScreen->findOriginalKeyID(pos); // Find the original key ID

		if (originalID) {
			keyFirstPos = *originalID;

			if (!hasKeyInInventory(*originalID)) {
				collectedKeys.push_back(*originalID); // Store unique key ID
			}

		}
	}
}

void Player::handleDoorInteraction(const Point& p_orig, char targetChar, Doors* currentDoor) {
	if (currentDoor != nullptr) {
		const SwitchBoard& switchBoard = theScreen->getSwitchBoard();
		// Attempt to deposit key into door
		if (hasElement() && getHeldElement() == 'K' && currentDoor->getRequiredKeyCount() > 0) {
			if (currentDoor->depositKey(keyFirstPos)) {
				consumeHeldKey(); // remove key from player's inventory and state
				keyFirstPos = Point(-1, -1, 0, 0, ' ');
			}
			p = p_orig;
			return;
		}

		// Check if door can be passed
		if (!currentDoor->canPlayerPass(switchBoard)) {
			p = p_orig; // Door is locked or requirements not met
		}
		else {
			// Handle screen transition logic
			char playerChar = p.getChar();
			if (currentDoor->getDestinationScreenID() != theScreen->getCurrentScreenID()) {
				currDoor = currentDoor;
				awaitingScreenTransition = true; // Signal the GameManager to change screens
			}
			else {
				// Internal door movement on the same screen (teleport)
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
		p = p_orig; // Invalid door character
	}
}

Point Player::getHeldElementPos() const {
	if (heldElement == 'K') {
		return keyFirstPos; // Return original key ID for key (stored in keyFirstPos)
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
			dir = Direction::RIGHT; // Default direction for disposal
		}
	}

	Point elementDropPos = p;
	elementDropPos.setDirection(dir);
	elementDropPos.move(); // Calculate drop location


	if (theScreen->isWall(elementDropPos) ||
		theScreen->isDoor(elementDropPos) ||
		theScreen->isSwitchOff(elementDropPos) ||
		theScreen->isSwitchOn(elementDropPos) ||
		theScreen->isRiddle(elementDropPos) ||
		theScreen->isTorch(elementDropPos) ||
		theScreen->isBomb(elementDropPos) ||
		theScreen->getCharAt(elementDropPos) != ' ') {
		return; // Cannot drop element here
	}

	if (heldElement == 'K') {
		removeKeyFromInventory(heldElementPos); // Remove key from collected keys
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

	theScreen->setCharAt(elementDropPos, heldElement); // Place element on map
	heldElement = ' ';
	p.setDirection(Direction::STAY);
	lastMoveDir = Direction::STAY;
	theScreen->draw();
	p.draw();
}

// Check if a key position is already in collectedKeys
bool Player::hasKeyInInventory(const Point& keyPos) const {
    for (const auto& storedKey : collectedKeys) {
        if (storedKey.getX() == keyPos.getX() && storedKey.getY() == keyPos.getY()) {
            return true;
        }
    }
    return false;
}

bool Player::removeKeyFromInventory(const Point& keyPos) { // Removes key from collected keys vector
	for (auto it = collectedKeys.begin(); it != collectedKeys.end(); ++it) {
		if (it->getX() == keyPos.getX() && it->getY() == keyPos.getY()) {
			collectedKeys.erase(it);
			return true;
		}
	}
	return false; // Key not found
}


// Try to pop a bomb request (set by disposing a bomb) and return its position
bool Player::tryPopBombRequest(Point& out) {
    if (!bombRequested) return false;
    out = bombRequestPos;
    bombRequested = false;
    return true;
}
