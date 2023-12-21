#ifndef AStar_h
#define AStar_h

#include <Arduino.h>
#include "globals/DataBounds.h"
#include "globals/SerialDebugging.h"
#include "globals/Typedefs.h"
#include "Coordinates.h"
#include "spi/Display.h"
#include "states/Play.h"

constexpr uint8_t sixBitMax = 63;

using SetType = uint8_t;
constexpr SetType openSet = 0;
constexpr SetType closedSet = 1;

class Node {
	public:
		Node() {
			reset();
		}
		void reset() {
			walkable = true;
			parent = nullptr;
			x = 0;
			y = 0;
			fCost = sixBitMax;
			gCost = sixBitMax;
			hCost = sixBitMax;
			resetSet();
		}

		void resetSet() {
			inSet[openSet] = false;
			inSet[closedSet] = false;
		}

		boolean walkable = true;
		Node* parent = nullptr; // used to follow path back from goal
		uint8_t x : 4;
		uint8_t y : 4;
		uint8_t gCost : 6; // the cost of moving from starting node to this node
								 // (aggregate via neighboring nodes)
		uint8_t hCost : 6; // always the distance to global goal
		uint8_t fCost : 6; // g + h

		boolean inSet[2];
};

namespace AStar {
	void allocate();
	void deallocate();
	void resetSets();
	void resetGrid();
	Node* getNode(ScreenPosition pos);
	uint8_t calculateDistance(Node* node1, Node* node2);
	Node* lowestFCostInOpenSet();
	void updateObstacles(uint8_t roomId);
	void drawWalkableNodes();
	uint8_t getNeighborNodes(Node* current, Node* neighbors[4]);
	void generateParentPath(Node* start, Node* goal);
	boolean nextMove(ScreenPosition* newPos, ScreenPosition currentPos, ScreenPosition targetPos);
} // namespace AStar

extern boolean debugPathfinding;
#endif