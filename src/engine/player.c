#include "player.h"
#include "viewport.h"
#include "cube.h"

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

void drawInHandItem() {
    glPushMatrix();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(0.5f, 0.0f, -0.5f);
	glRotatef(20.0f, 0.0f, 1.0f, 0.0f);
	glRotatef(-15.0f, 1.0f, 0.0f, 0.0f);
    glScalef(0.6f, 0.8f, 0.8f);

    drawCube((Vector3) { -0.5f, 0.0f, -0.5f }, getColorByType(1));

    glPopMatrix();
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
