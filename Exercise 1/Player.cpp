#include "Player.h"
#include "Point.h"
#include "Doors.h"
#include "GameManager.h"
#include "MapChar.h"
#include <cctype>
#include <cstring>

Player::Player(const Point& point, const char(&keys)[NUM_KEYS + 1], screen* screen, GameManager* gm) :
	theScreen(screen), p(point), gameManager(gm){
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
			if (dir != p.getDirection() || dir == Direction::STAY) {
				Spring* s = theScreen->getSpringByPosition(p);
				if (s && s->isCompressed()) {
					initiateLaunch(s);
					return;
				}
			}
			p.setDirection(dir);
			//if (dir != Direction::STAY) {
			lastMoveDir = dir; // Remember last moving direction
			//}
			return;

		}
	}
}

void Player::consumeHeldKey() {
	if (heldElement == static_cast<char>(MapChar::Key)) {
		removeKeyFromInventory(keyFirstPos);
		heldElement = static_cast<char>(MapChar::Empty);
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

	if (isBeingLaunched) {
		handleSpringLaunch(); 
		return;
	}

	if (ticksUntilNextMove > 0) { // Check if movement interval has passed
		--ticksUntilNextMove;
		return;
	}

	ticksUntilNextMove = MOVE_TICK_INTERVAL;

	char backgroundChar = theScreen->getCharAt(p);
	p.draw(backgroundChar); // Redraw background at current position
	Point p_orig = p;
	p.move(); // Calculate new position
	
	handleInteractions(p_orig);

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

bool Player::hasElement() const {
	return heldElement != static_cast<char>(MapChar::Empty);
}

bool Player::hasTorch() const {
	return heldElement == static_cast<char>(MapChar::Torch);
}

void Player::pickUpElement(char element, const Point& pos) {
	heldElement = element;
	heldElementPos = pos;

	if (element == static_cast<char>(MapChar::Key)) {
		const Point* originalID = theScreen->findOriginalKeyID(pos);
		if (originalID) {
			keyFirstPos = *originalID;
			if (!hasKeyInInventory(*originalID)) {
				collectedKeys.push_back(*originalID);
			}
		}
	}
}

void Player::handleDoorInteraction(const Point& p_orig, char targetChar, Doors* currentDoor) {
	if (currentDoor != nullptr) {
		const SwitchBoard& switchBoard = theScreen->getSwitchBoard();
		// Attempt to deposit key into door
		if (hasElement() && getHeldElement() == static_cast<char>(MapChar::Key) && currentDoor->getRequiredKeyCount() > 0) {
			if (currentDoor->depositKey(keyFirstPos)) {
				consumeHeldKey(); // remove key from player's inventory and state
				keyFirstPos = Point(-1, -1, 0, 0, static_cast<char>(MapChar::Empty));
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
				theScreen->setCharAt(p, static_cast<char>(MapChar::Empty));
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
	if (heldElement == static_cast<char>(MapChar::Key)) {
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
	if (heldElement == static_cast<char>(MapChar::Empty))
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
		theScreen->getCharAt(elementDropPos) != static_cast<char>(MapChar::Empty)) {
		return; // Cannot drop element here
	}

	if (heldElement == static_cast<char>(MapChar::Key)) {
		removeKeyFromInventory(heldElementPos); // Remove key from collected keys
	}

	if (heldElement == static_cast<char>(MapChar::BombItem)) {
		bombRequested = true;
		bombRequestPos = elementDropPos;
		theScreen->setCharAt(elementDropPos, static_cast<char>(MapChar::BombItem));
		heldElement = static_cast<char>(MapChar::Empty);
		p.setDirection(Direction::STAY);
		lastMoveDir = Direction::STAY;
		theScreen->draw();
		p.draw();
		return;
	}

	theScreen->setCharAt(elementDropPos, heldElement); // Place element on map
	heldElement = static_cast<char>(MapChar::Empty);
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

bool Player::tryPopBombRequest(Point& out) {
	if (!bombRequested) return false;
	out = bombRequestPos;
	bombRequested = false;
	return true;
}

bool Player::ifPlayerCanPressSpring(Spring* currSpring) const {
	if (p.getDirection() == Direction::RIGHT && currSpring->getReleaseDirection() == Direction::LEFT) {
		return true;
	}
	else if (p.getDirection() == Direction::LEFT && currSpring->getReleaseDirection() == Direction::RIGHT) {
		return true;
	}
	else if (p.getDirection() == Direction::UP && currSpring->getReleaseDirection() == Direction::DOWN) {
		return true;
	}
	else if (p.getDirection() == Direction::DOWN && currSpring->getReleaseDirection() == Direction::UP) {
		return true;
	}
	return false;
}

void Player::initiateLaunch(Spring* s) {
	this->springSpeed = s->getReleaseSpeed();
	this->springTimer = s->getReleaseDuration();
	this->activeSpringDir = s->getReleaseDirection();
	this->isBeingLaunched = true;
	this->setDirection(activeSpringDir);

	this->springToReset = s;
}

void Player::handleSpringLaunch() {
	char backgroundUnderPlayer = theScreen->getCharAt(p);
	p.draw(backgroundUnderPlayer);

	Direction inputDir = this->p.getDirection(); 

	if (isPerpendicular(inputDir, activeSpringDir)) {
		Point lateralPos = p.calculateNext(inputDir);

		if (theScreen->isClear(lateralPos, true) && !gameManager->isOtherPlayerAt(lateralPos, this)) {
			p = lateralPos;
		}
	}

	for (int i = 0; i < this->springSpeed; ++i) { 
		Point nextPos = p.calculateNext(activeSpringDir);

		if (gameManager->isOtherPlayerAt(nextPos, this)) {
			gameManager->transferLaunch(this, nextPos);
			this->springTimer = 0;
			this->isBeingLaunched = false;
			this->p.setDirection(Direction::STAY);
			this->lastMoveDir = Direction::STAY;
			return;
		}
		Point originalPos = p;
		if (theScreen->isClear(nextPos, true)) {
			p = nextPos;
			handleInteractions(originalPos);
			if (p == originalPos) {
				break;
			}
			if (!isBeingLaunched) {
				return; 
			}
		}
		else {
			this->springTimer = 0; 
			this->isBeingLaunched = false;
			this->springSpeed = 1;
			this->p.setDirection(Direction::STAY);
			this->lastMoveDir = Direction::STAY;
			p.draw(); 
			return;
		}
	}

	if (this->springToReset != nullptr) {
		this->springToReset->resetSpring();
		theScreen->draw(); 
		this->springToReset = nullptr; 
	}

	this->springTimer--; 
	if (this->springTimer <= 0) {
		this->isBeingLaunched = false;
		this->springSpeed = 1;
		if (isPerpendicular(inputDir, activeSpringDir)) {
			this->p.setDirection(inputDir);
			this->lastMoveDir = inputDir;
		}
		else {
			this->p.setDirection(Direction::STAY);
			this->lastMoveDir = Direction::STAY;
		}
	}
	p.draw();
}

bool Player::isPlayerKey(char key) const {
	for (size_t index = 0; index < NUM_KEYS; ++index) {
		char k = the_keys[index];
		if (k == key) {
			return true;
		}
	}
	return false;
}


Direction Player::getDirectionFromKey(char key) const {
	for (size_t index = 0; index < NUM_KEYS; ++index) {
		char k = the_keys[index];
		if (k == key) {
			return static_cast<Direction>(index);
		}
	}
	return Direction::STAY;
}

bool Player::isPerpendicular(Direction dir1, Direction dir2)  const{
	if ((dir1 == Direction::UP || dir1 == Direction::DOWN) &&
		(dir2 == Direction::LEFT || dir2 == Direction::RIGHT)) {
		return true;
	}
	if ((dir1 == Direction::LEFT || dir1 == Direction::RIGHT) &&
		(dir2 == Direction::UP || dir2 == Direction::DOWN)){ 
		return true;
	}
	return false;
}

void Player::receiveLaunch(Direction dir, int speed, int timer, Direction lateralDir) {
	this->isBeingLaunched = true;
	this->activeSpringDir = dir;
	this->springSpeed = speed;
	this->springTimer = timer;
	this->p.setDirection(lateralDir);
	this->lastMoveDir = lateralDir;

	this->springToReset = nullptr;
}

void Player::handleInteractions(const Point& p_orig) {
	if (theScreen->isWall(p)) {
		Spring* theSpring = theScreen->getSpringByPosition(p_orig);
		if (theSpring && theSpring->isCompressed()) {
			p = p_orig;
			initiateLaunch(theSpring);
			return;
		}
		else {
			p = p_orig;
		}
	}
	else if (theScreen->isDoor(p)) {
		char targetChar = theScreen->getCharAt(p);
		Doors* currentDoor = theScreen->getDoorByChar(targetChar);
		handleDoorInteraction(p_orig, targetChar, currentDoor); // Handle door logic (pass/deposit/block)
	}
	else if (theScreen->getCharAt(p) == static_cast<char>(MapChar::BombItem) && theScreen->isBombArmedAt(p)) {
		p = p_orig;  // active bomb on the ground(counting down to explode) � block stepping onto this tile
	}

	else if (theScreen->isSwitchOff(p) || theScreen->isSwitchOn(p)) {
		bool steppedOntoSwitch = (p.getX() != p_orig.getX()) || (p.getY() != p_orig.getY());
		if (steppedOntoSwitch) {
			theScreen->toggleSwitchAt(p); // Toggle switch state
			if (isBeingLaunched) {
				theScreen->draw();
			}
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
		theScreen->setCharAt(p, static_cast<char>(MapChar::Empty)); // Remove element from map
		theScreen->draw();
		p.draw();
	}
	else if (theScreen->isRiddle(p)) {
		Riddle* currentRiddle = theScreen->getRiddleByPosition(p);
		p = p_orig; // Stop on riddle, movement handled by control unit
		if (currentRiddle) {
			setActiveRiddle(currentRiddle);// Signal riddle solving sequence
			if (isBeingLaunched) {
				this->isBeingLaunched = false;
				this->springTimer = 0;
			}
		}

	}
	else if (theScreen->isSpring(p)) {
		if (isBeingLaunched) {
			this->isBeingLaunched = false;
			this->springTimer = 0;
			p = p_orig;
			return;
		}
		Spring* currentSpring = theScreen->getSpringByPosition(p);
		if (ifPlayerCanPressSpring(currentSpring)) {
			currentSpring->compress();
		}
		else {
			p = p_orig; // Cannot use spring, revert position
		}
	}
	else if (theScreen->isObstacle(p)) {
		Obstacle* currentObstacle = theScreen->getObstacleByPosition(p);
		if (currentObstacle) {
			Direction pushDir = (isBeingLaunched) ? activeSpringDir : lastMoveDir;
			if (gameManager->canObstacleMove(currentObstacle, pushDir, this)) {
				Obstacle* secondObs = nullptr;
				for (const auto& part : currentObstacle->getPositions()) {
					Point np = part.calculateNext(pushDir);
					if (theScreen->isObstacle(np)) {
						secondObs = theScreen->getObstacleByPosition(np);
						if (secondObs == currentObstacle) secondObs = nullptr;
						else break;
					}
				}
				if (secondObs) secondObs->moveObstacle(pushDir);
				currentObstacle->moveObstacle(pushDir);
				if (isBeingLaunched && (theScreen->isSwitchOff(p) || theScreen->isSwitchOn(p))) {
					theScreen->draw();
				}
			}
			else {
				p = p_orig; // Obstacle cannot be moved, revert position
				lastMoveDir = Direction::STAY;
				if (isBeingLaunched) {
					this->isBeingLaunched = false;
					this->springTimer = 0;
				}
			}
		}
	}
}
