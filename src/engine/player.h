#ifndef BLOCKS_PLAYER
#define BLOCKS_PLAYER

#include <stdbool.h>

#include "types.h"

typedef struct PlayerState {
	Vector3 position;
	Vector3 rotation;
	RelativeVector3 forces;
	double height;
	double speed;
	bool inAir;
	Vector3* lookingAtBlock;
	bool isLookingAtBlock;
	Vector3 lookingAtBlockSurfacePoint;
} PlayerState;

PlayerState getPlayerState();
void playerFollowViewport();
void appendPlayerForces(RelativeVector3 forces);
void setPlayerInAirState(bool state);
void setPlayerForces(RelativeVector3 forces);
void drawInHandItem();
void updateLookingAtBlock();
int getBlockFace();

#endif
