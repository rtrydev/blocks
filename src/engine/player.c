#include "player.h"
#include "viewport.h"

PlayerState currentPlayerState = {
	.position = {
		.x = 0.0,
		.y = 0.0,
		.z = 0.0
	},
	.rotation = {
		.x = 0.0,
		.y = 0.0,
		.z = 0.0
	},
	.forces = {
		.forward = 0.0,
		.upward = -2.0,
		.sideway = 0.0
	},
	.height = 1.8,
	.speed = 0.004,
	.inAir = false
};

PlayerState getPlayerState() {
	return currentPlayerState;
}

void playerFollowViewport() {
	Vector3 position = getViewportPosition();
	Vector3 rotation = getViewportRotation();

	currentPlayerState.position = position;
	currentPlayerState.rotation = rotation;
}

void appendPlayerForces(RelativeVector3 forces) {
	currentPlayerState.forces.forward += forces.forward;
	currentPlayerState.forces.upward += forces.upward;
	currentPlayerState.forces.sideway += forces.sideway;
}

void setPlayerForces(RelativeVector3 forces) {
	currentPlayerState.forces = forces;
}

void setPlayerInAirState(bool state) {
	currentPlayerState.inAir = state;
}
