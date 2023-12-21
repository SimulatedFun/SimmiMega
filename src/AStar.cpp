#include "AStar.h"

namespace AStar {
	Node* grid[13][10];
	uint8_t setLength[2];

	Node* getNode(ScreenPosition pos) {
		return grid[pos.x][pos.y];
	}

	void allocate() {
		for (uint8_t x = 0; x < 13; x++) {
			for (uint8_t y = 0; y < 10; y++) {
				grid[x][y] = new Node();
			}
		}
	}

	void deallocate() {
		for (uint8_t x = 0; x < 13; x++) {
			for (uint8_t y = 0; y < 10; y++) {
				delete grid[x][y];
			}
		}
	}

	void resetSets() {
		setLength[openSet] = 0;
		setLength[closedSet] = 0;
		for (uint8_t x = 0; x < 13; x++) {
			for (uint8_t y = 0; y < 10; y++) {
				grid[x][y]->resetSet();
				grid[x][y]->hCost = 63;
				grid[x][y]->fCost = 63;
				grid[x][y]->gCost = 63;
			}
		}
	}

	void resetGrid() {
		DEBUG(F("reset grid"));
		for (uint8_t x = 0; x < 13; x++) {
			for (uint8_t y = 0; y < 10; y++) {
				grid[x][y]->reset();
				grid[x][y]->x = x;
				grid[x][y]->y = y;
			}
		}
	}

	uint8_t calculateDistance(Node* node1, Node* node2) {
		return abs(node1->x - node2->x) + abs(node1->y - node2->y);
	}

	void addToSet(Node* node, SetType type) {
		node->inSet[type] = true;
		setLength[type]++;
	}

	void removeFromSet(Node* node, SetType type) {
		if (node->inSet[type]) {
			node->inSet[type] = false;
			setLength[type]--;
		}
	}

	boolean inSet(Node* node, SetType type) {
		return node->inSet[type];
	}

	Node* lowestFCostInOpenSet() {
		Node* lowest = nullptr;

		// finds one node to start as the lowest node
		for (uint8_t i = 0; i < 130; i++) {
			const uint8_t x = i % 13;
			const uint8_t y = i / 13;
			if (grid[x][y] == nullptr) {
				continue;
			}
			if (grid[x][y]->inSet[openSet]) {
				lowest = grid[x][y];
				break;
			}
		}

		// checks against all the other nodes
		for (uint8_t i = 0; i < 130; i++) {
			const uint8_t x = i % 13;
			const uint8_t y = i / 13;
			if (grid[x][y] == nullptr) {
				continue;
			}

			// node exists

			if (!grid[x][y]->inSet[openSet]) {
				continue;
			}

			// in open set

			if (grid[x][y]->fCost < lowest->fCost) {
				lowest = grid[x][y];
			}
		}

		return lowest;
	}

	void updateObstacles(uint8_t roomId) {
		INFO(F("updateObstacles for room ") << roomId);
		for (uint8_t x = 0; x < 13; x++) {
			for (uint8_t y = 0; y < 10; y++) {
				const Coordinates coords(x, y, roomId);
				if (Play::isAreaClear(coords)) {
					grid[x][y]->walkable = true;
				} else {
					grid[x][y]->walkable = false;
				}
			}
		}
	}

	void drawWalkableNodes() {
		Serial << "drawWalkableNodes:" << endl;
		for (uint8_t y = 0; y < 10; y++) {
			for (uint8_t x = 0; x < 13; x++) {
				if (grid[x][y]->walkable) {
					Serial << ". ";
					display->fillRectangle(x * 24 + 10, y * 24 + 10, 4, 4, DARK_GREY);
				} else {
					Serial << "# ";
					display->fillRectangle(x * 24 + 10, y * 24 + 10, 4, 4, BLACK);
				}
			}
			Serial << endl;
		}
		Serial << endl;
	}

	/// Function to retrieve neighboring nodes of a given node
	/// Returns number of neighbors
	uint8_t getNeighborNodes(Node* current, Node* neighbors[4]) {
		uint8_t neighborCount = 0;
		const uint8_t x = current->x;
		const uint8_t y = current->y;

		// Check left neighbor
		if (x > 0) {
			neighbors[neighborCount] = grid[x - 1][y];
			neighborCount++;
		}

		// Check right neighbor
		if (x < 12) {
			neighbors[neighborCount] = grid[x + 1][y];
			neighborCount++;
		}

		// Check above neighbor
		if (y > 0) {
			neighbors[neighborCount] = grid[x][y - 1];
			neighborCount++;
		}

		// Check below neighbor
		if (y < 9) {
			neighbors[neighborCount] = grid[x][y + 1];
			neighborCount++;
		}

		return neighborCount;
	}

	Node* lowestHCostInCloseSet() {
		Node* lowest = nullptr;

		// finds one node to start as the lowest node
		for (uint8_t i = 0; i < 130; i++) {
			const uint8_t x = i % 13;
			const uint8_t y = i / 13;
			if (grid[x][y] == nullptr) {
				continue;
			}
			if (grid[x][y]->inSet[closedSet]) {
				lowest = grid[x][y];
				break;
			}
		}

		// checks against all the other nodes
		for (uint8_t i = 0; i < 130; i++) {
			const uint8_t x = i % 13;
			const uint8_t y = i / 13;
			if (grid[x][y] == nullptr) {
				continue;
			}

			// node exists

			if (!grid[x][y]->inSet[closedSet]) {
				continue;
			}

			// in closed set

			if (grid[x][y]->hCost < lowest->hCost) {
				lowest = grid[x][y];
			}
		}

		return lowest;
	}

	// starts from goal and pathfinds to start, making a parent path from goal->parent pointing to start
	void generateParentPath(Node* start, Node* goal) {
		// make the start and end goals walkable so it can generate a pathway
		const boolean originalStartWalkable = start->walkable;
		const boolean originalGoalWalkable = goal->walkable;
		start->walkable = true;
		goal->walkable = true;

		// erases the old path if one can't be found
		start->parent = nullptr;
		goal->parent = nullptr;

		resetSets();
		addToSet(start, openSet);

		while (setLength[openSet] > 0) {
			Node* current = lowestFCostInOpenSet();
			removeFromSet(current, openSet);
			addToSet(current, closedSet);

			if (current == goal) {
				if (current == nullptr) {
					ERROR(F("current = null"));
				}
				//GOOD(F("path found from (") << start->x << ", " << start->y << ") to (" << goal->x << ", " << goal->y << ")");
				start->walkable = originalStartWalkable;
				goal->walkable = originalGoalWalkable;
				return; // path has been found, follow parent pointers from goal to start
			}

			Node* neighbors[4]; // only cardinal directions (traditionally includes diagonal traversal)
			const uint8_t neighborCount = getNeighborNodes(current, neighbors);
			for (uint8_t i = 0; i < neighborCount; i++) {
				Node* neighbor = neighbors[i];
				if (!neighbor->walkable) {
					continue;
				}
				if (inSet(neighbor, closedSet)) {
					continue;
				}

				// Calculate tentative g_cost for this neighbor
				const uint8_t tentativeGCost = current->gCost + calculateDistance(current, neighbor);

				// If neighbor is not in open set or the new path is better
				// (i.e. we haven't fully visited this node yet OR the pathway through the current to
				// this neighbor would result in a shorter total path to the goal)
				if (!inSet(neighbor, openSet) || tentativeGCost < neighbor->gCost) {

					// Update neighbor's costs
					neighbor->gCost = tentativeGCost;
					neighbor->hCost = calculateDistance(neighbor, goal);
					neighbor->fCost = neighbor->gCost + neighbor->hCost;
					neighbor->parent = current;

					// If neighbor is not in open set, add it
					if (!inSet(neighbor, openSet)) {
						addToSet(neighbor, openSet);
					}
				}
			}
		}
		DEBUG(F("exited path while w/o path"));
		start->walkable = originalStartWalkable;
		goal->walkable = originalGoalWalkable;
	}

	void reverse(Node* start, Node* end) {
		Node* cursor = start;
		Node* prev = nullptr;
		while (cursor != end) {
			// follow a parent
			Node* next = cursor->parent;
			cursor->parent = prev;
			prev = cursor;
			cursor = next;
		}
		cursor->parent = prev;
	}

	boolean nextMove(ScreenPosition* newPos, ScreenPosition currentPos, ScreenPosition targetPos) {
		if (currentPos.x == targetPos.x and currentPos.y == targetPos.y) {
			DEBUG(F("already at goal"));
			return false; // already at goal
		}

		// todo don't pathfind if the backup goal is farther than the current position

		Node* current = AStar::getNode(currentPos);
		Node* target = AStar::getNode(targetPos);

		DEBUG(F("nextMove, current: (")
				<< currentPos.x << ", " << currentPos.y << "), goal: " << targetPos.x << ", " << targetPos.y);
		AStar::generateParentPath(current, target); // from goal->parent to current

		Node* nextMove = target->parent;
		Node* backupTarget = nullptr;

		if (nextMove == nullptr) {
			DEBUG(F("no direct path to player"));
			backupTarget = lowestHCostInCloseSet(); // backup goal
			if (backupTarget == nullptr) {
				ERROR(F("backup goal null"));
				return false;
			}

			// skip a backup goal if it's farther than current position
			if (calculateDistance(current, target) <= calculateDistance(backupTarget, target)) {
				DEBUG(F("closer than backup target"));
				return false;
			}

			DEBUG("try to get closer, like from: (" << current->x << ", " << current->y << ") to -> ("
																 << backupTarget->x << ", " << backupTarget->y
																 << ")");

			AStar::generateParentPath(current, backupTarget);
			nextMove = backupTarget->parent;
			if (nextMove == nullptr) {
				ERROR(F("next move null"));
				return false;
			}
			target = backupTarget;
		}

		reverse(target, current);

		if (debugPathfinding) {
			Node* move = current;
			while (move != target) {
				display->fillRectangle(move->x * 24 + 10, move->y * 24 + 14, 4, 4, CMYK_MAGENTA);
				move = move->parent;
			}
			move = current;
			while (move != target) {
				display->fillRectangle(move->x * 24 + 10, move->y * 24 + 14, 4, 4, DARK_BLUE);
				move = move->parent;
			}
		}

		// Since it generates a linked list from start to end (our object to the player)
		// if you just reverse the two, and generate it from the player to the object, then the first
		// node's parent is the first move to make
		nextMove = current->parent; // current->parent now points towards goal

		if (nextMove == nullptr) {
			DEBUG(F("no path"));
			return false; // valid, just no path found
		}

		newPos->x = nextMove->x;
		newPos->y = nextMove->y;
		return true;
	}
} // namespace AStar