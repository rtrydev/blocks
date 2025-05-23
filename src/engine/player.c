#include "player.h"
#include "viewport.h"
#include "cube.h"
#include "world.h"
#include <math.h>
// gametime.h is not directly needed for raycasting logic itself.

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
	.inAir = false,
	.lookingAtBlock = NULL,
	.isLookingAtBlock = false
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

void updateLookingAtBlock() {
    PlayerState ps = getPlayerState(); // Get a copy of current state

    // Initialize before ray casting
    currentPlayerState.isLookingAtBlock = false;
    currentPlayerState.lookingAtBlock = NULL;

    Vector3 rayOrigin;
    rayOrigin.x = ps.position.x;
    rayOrigin.y = ps.position.y + ps.height - 1.0f; // Adjust to eye level
    rayOrigin.z = ps.position.z;

    Vector3 viewDirection = ps.rotation; // This is already a normalized direction vector

    WorldState* ws = getWorldStateGlobal();
    if (!ws) {
        // Cannot proceed if world state is not available
        return;
    }

    const float maxDistance = 2.5f; // Max distance to check for blocks
    const float step = 0.1f;      // Step size for ray casting

    for (float distance = 0.5f; distance <= maxDistance; distance += step) {
        Vector3 checkPos;
        checkPos.x = rayOrigin.x + viewDirection.x * distance;
        checkPos.y = rayOrigin.y + viewDirection.y * distance;
        checkPos.z = rayOrigin.z + viewDirection.z * distance;

        int blockX = floorf(checkPos.x);
        int blockY = floorf(checkPos.y);
        int blockZ = floorf(checkPos.z);

        GameElement* block = getBlockAtGlobal(ws, blockX, blockY, blockZ);

        if (block != NULL && block->elementType != 0) { // elementType 0 is usually air
            Vector3 blockCenter;
            blockCenter.x = block->position.x + 0.5f;
            blockCenter.y = block->position.y + 0.5f;
            blockCenter.z = block->position.z + 0.5f;

            // Calculate distance from rayOrigin (eye level) to the center of the block
            float dx = blockCenter.x - rayOrigin.x;
            float dy = blockCenter.y - rayOrigin.y;
            float dz = blockCenter.z - rayOrigin.z;
            float actualDistanceToBlockCenter = sqrtf(dx*dx + dy*dy + dz*dz);

            if (actualDistanceToBlockCenter <= 2.0f) {
                currentPlayerState.isLookingAtBlock = true;
                // Store a pointer to the block's position.
                // This assumes GameElement's position remains valid and accessible.
                currentPlayerState.lookingAtBlock = (Vector3*)&block->position;
            } else {
                // First block intersected is too far.
                currentPlayerState.isLookingAtBlock = false;
                currentPlayerState.lookingAtBlock = NULL;
            }
            return; // Exit after checking the first intersected block (whether near or far)
        }
    }
    // If loop completes, no block was found within maxDistance along the ray.
    // isLookingAtBlock is already false, lookingAtBlock is already NULL.
}
