#ifndef BLOCKS_WORLD
#define BLOCKS_WORLD

#include <stdbool.h>

void generateWorld(int**** worldStructure);
void removeWorld(int**** worldStructure);
void drawWorld(int*** worldStructure);

void searchForCollision(
	double positionX, double positionY, double positionZ,
	double directionX, double directionY, double directionZ,
	bool* isCollisionX, bool* isCollisionY, bool* isCollisionZ,
	int**** worldStructure
);

#endif