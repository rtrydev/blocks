#include "player.h"
#include "viewport.h"
#include "cube.h"
#include "world.h"
#include "constants.h"
#include <math.h>

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

    drawCube((Vector3) { -0.5f, 0.0f, -0.5f }, getColorByType(1), DEFAULT_OUTLINE_COLOR);

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
    PlayerState ps = getPlayerState();

    currentPlayerState.isLookingAtBlock = false;
    currentPlayerState.lookingAtBlock = NULL;
    currentPlayerState.lookingAtBlockSurfacePoint = (Vector3){0.0f, 0.0f, 0.0f};

    Vector3 rayOrigin;
    rayOrigin.x = ps.position.x;
    rayOrigin.y = ps.position.y + ps.height + 0.5f;
    rayOrigin.z = ps.position.z;

    Vector3 viewDirection = getViewportRotation();

    WorldState* ws = getWorldStateGlobal();
    if (!ws) {
        return;
    }

    const float maxDistance = 3.5f;
    const float step = 0.1f;

    for (float distance = 0.0f; distance <= maxDistance; distance += step) {
        Vector3 checkPos;
        checkPos.x = rayOrigin.x + viewDirection.x * distance;
        checkPos.y = rayOrigin.y + viewDirection.y * distance;
        checkPos.z = rayOrigin.z + viewDirection.z * distance;

        int blockX = floorf(checkPos.x);
        int blockY = floorf(checkPos.y);
        int blockZ = floorf(checkPos.z);

        GameElement* block = getBlockAtGlobal(ws, blockX, blockY, blockZ);

        if (block != NULL && block->elementType != 0) {
            Vector3 blockCenter;
            blockCenter.x = block->position.x + 0.5f;
            blockCenter.y = block->position.y + 0.5f;
            blockCenter.z = block->position.z + 0.5f;

            float dx = blockCenter.x - rayOrigin.x;
            float dy = blockCenter.y - rayOrigin.y;
            float dz = blockCenter.z - rayOrigin.z;
            float actualDistanceToBlockCenter = sqrtf(dx*dx + dy*dy + dz*dz);

            if (actualDistanceToBlockCenter <= 4.0f) {
                currentPlayerState.isLookingAtBlock = true;
                currentPlayerState.lookingAtBlock = (Vector3*)&block->position;
                currentPlayerState.lookingAtBlockSurfacePoint = checkPos;
            } else {
                currentPlayerState.isLookingAtBlock = false;
                currentPlayerState.lookingAtBlock = NULL;
            }
            return;
        }
    }
}

int getBlockFace() {
    if (!currentPlayerState.isLookingAtBlock || currentPlayerState.lookingAtBlock == NULL) {
        return BLOCK_FACE_NONE;
    }

    Vector3 intersectionPoint = currentPlayerState.lookingAtBlockSurfacePoint;
    Vector3 targetBlockIntegerCoordinates = *currentPlayerState.lookingAtBlock;

    float blockCenterX = targetBlockIntegerCoordinates.x + 0.5f;
    float blockCenterY = targetBlockIntegerCoordinates.y + 0.5f;
    float blockCenterZ = targetBlockIntegerCoordinates.z + 0.5f;

    float hitNormalX = intersectionPoint.x - blockCenterX;
    float hitNormalY = intersectionPoint.y - blockCenterY;
    float hitNormalZ = intersectionPoint.z - blockCenterZ;

    float absX = fabsf(hitNormalX);
    float absY = fabsf(hitNormalY);
    float absZ = fabsf(hitNormalZ);

    float maxVal = absX;
    int face = BLOCK_FACE_LEFT;
    if (hitNormalX > 0) {
        face = BLOCK_FACE_RIGHT;
    }

    if (absY > maxVal) {
        maxVal = absY;
        face = BLOCK_FACE_BOTTOM;
        if (hitNormalY > 0) {
            face = BLOCK_FACE_TOP;
        }
    }

    if (absZ > maxVal) {
        face = BLOCK_FACE_BACK;
        if (hitNormalZ > 0) {
            face = BLOCK_FACE_FRONT;
        }
    }

    return face;
}
