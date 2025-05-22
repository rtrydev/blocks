#include "forces.h"
#include "viewport.h"
#include "gametime.h"
#include "userinputs.h"
#include "constants.h"
#include "player.h"
#include "world.h"

#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

double previousPlayerPositionY = 0.0;
bool playerPositionInitialized = false;
bool blockJump = false;

bool collisionOnX = false;
bool collisionOnY = false;
bool collisionOnZ = false;

void addForcesBasedOnInputs() {
    PlayerState playerState = getPlayerState();

    RelativeVector3 forces = {
        .forward = 0.0,
        .sideway = 0.0,
        .upward = playerState.forces.upward
    };

    InputState inputState = getInputState();

    if (inputState.UP_ACTIVE) {
        forces.forward += 1.0;
    }

    if (inputState.DOWN_ACTIVE) {
        forces.forward -= 1.0;
    }

    if (inputState.RIGHT_ACTIVE) {
        forces.sideway += 1.0;
    }

    if (inputState.LEFT_ACTIVE) {
        forces.sideway -= 1.0;
    }

    setPlayerForces(forces);

    if (inputState.JUMP_ACTIVE) {
        if (!playerState.inAir && !blockJump) {
            blockJump = true;
            RelativeVector3 jumpForce = {
                .forward = 0.0,
                .sideway = 0.0,
                .upward = 4.4
            };

            appendPlayerForces(jumpForce);
        }
    }
}

void adjustForcesBasedOnCollision() {
    PlayerState playerState = getPlayerState();
    Vector3 position = getViewportPosition();
    Vector3 rotation = getViewportRotation();

    GameElement* nearGameElements = NULL;
    Vector3 rangeTo = {
        .x = 1.5,
        .y = 1.0 + playerState.height,
        .z = 1.5
    };
    Vector3 rangeFrom = {
        .x = 1.5,
        .y = 1.0,
        .z = 1.5
    };

    getGameElementsInProximity(position, rangeFrom, rangeTo, &nearGameElements);

    if (nearGameElements == NULL) {
        return;
    }

    RelativeVector3 collisionForces = {
        .forward = 0.0,
        .upward = 0.0,
        .sideway = 0.0
    };

    double forceVectorLength = sqrt(pow(playerState.forces.forward, 2) + pow(playerState.forces.sideway, 2));

    double normalizedForward = playerState.speed * playerState.forces.forward / forceVectorLength;
    double normalizedSideway = playerState.speed * playerState.forces.sideway / forceVectorLength;

    double directionVectorLength = sqrt(pow(rotation.x, 2) + pow(rotation.z, 2));
    double normalizedLookAtX = rotation.x / directionVectorLength;
    double normalizedLookAtZ = rotation.z / directionVectorLength;

    double xPositionChange = 0.0;
    double yPositionChange = playerState.speed * playerState.forces.upward * deltaTime() / 2.0;
    double zPositionChange = 0.0;

    if (forceVectorLength > EPSILON) {
        xPositionChange =
            normalizedForward * normalizedLookAtX * deltaTime()
            + normalizedSideway * (normalizedLookAtX * cos(PI / 2.0) - normalizedLookAtZ * sin(PI / 2.0)) * deltaTime();

        zPositionChange =
            normalizedForward * normalizedLookAtZ * deltaTime()
            + normalizedSideway * (normalizedLookAtX * sin(PI / 2.0) + normalizedLookAtZ * cos(PI / 2.0)) * deltaTime();
    }

    Vector3 positionAfterTransaction = {
        .x = position.x + xPositionChange,
        .y = position.y + yPositionChange,
        .z = position.z + zPositionChange
    };

    collisionOnX = false;
    collisionOnY = false;
    collisionOnZ = false;

    for (int i = 0; i < 36; i++) {
        GameElement element = nearGameElements[i];

        if (element.elementType == 0) {
            continue;
        }

        bool allignsOnX = position.x + 0.25 > element.position.x && position.x - 1.25 < element.position.x;
        bool allignsOnY = position.y - EPSILON < element.position.y && position.y + playerState.height - EPSILON > element.position.y - 1.0;
        bool allignsOnZ = position.z + 0.25 > element.position.z && position.z - 1.25 < element.position.z;

        if (!collisionOnX && allignsOnZ && allignsOnY
            && positionAfterTransaction.x + 0.25 > element.position.x && positionAfterTransaction.x - 1.25 < element.position.x
        ) {
            collisionOnX = true;
        }

        if (!collisionOnY && allignsOnX && allignsOnZ) {
            if (position.y < element.position.y) {
                collisionOnY = positionAfterTransaction.y + playerState.height + EPSILON > element.position.y - 1.0;
            }
            else {
                collisionOnY = positionAfterTransaction.y - EPSILON < element.position.y;
            }
        }

        if (!collisionOnZ && allignsOnX && allignsOnY
            && positionAfterTransaction.z + 0.25 > element.position.z && positionAfterTransaction.z - 1.25 < element.position.z
        ) {
            collisionOnZ = true;
        }
    }

    free(nearGameElements);
}

void processForces() {
    PlayerState playerState = getPlayerState();
    Vector3 position = getViewportPosition();
    Vector3 rotation = getViewportRotation();

    if (!playerPositionInitialized) {
        previousPlayerPositionY = position.y;
        playerPositionInitialized = true;

        return;
    }

    double forceVectorLength = sqrt(pow(playerState.forces.forward, 2) + pow(playerState.forces.sideway, 2));

    double normalizedForward = playerState.speed * playerState.forces.forward / forceVectorLength;
    double normalizedSideway = playerState.speed * playerState.forces.sideway / forceVectorLength;

    double directionVectorLength = sqrt(pow(rotation.x, 2) + pow(rotation.z, 2));
    double normalizedLookAtX = rotation.x / directionVectorLength;
    double normalizedLookAtZ = rotation.z / directionVectorLength;

    double xPositionChange = 0.0;
    double zPositionChange = 0.0;

    if (forceVectorLength > EPSILON) {
        xPositionChange =
            normalizedForward * normalizedLookAtX * deltaTime()
            + normalizedSideway * (normalizedLookAtX * cos(PI / 2.0) - normalizedLookAtZ * sin(PI / 2.0)) * deltaTime();

        zPositionChange =
            normalizedForward * normalizedLookAtZ * deltaTime()
            + normalizedSideway * (normalizedLookAtX * sin(PI / 2.0) + normalizedLookAtZ * cos(PI / 2.0)) * deltaTime();
    }

    position.x += collisionOnX ? 0.0 : xPositionChange;
    position.z += collisionOnZ ? 0.0 : zPositionChange;

    if (playerState.inAir && position.y == previousPlayerPositionY) {
        setPlayerInAirState(false);

        RelativeVector3 forces = {
            .forward = playerState.forces.forward,
            .sideway = playerState.forces.sideway,
            .upward = -1.0
        };

        setPlayerForces(forces);
        blockJump = false;
    }

    if (!playerState.inAir && position.y != previousPlayerPositionY) {
        setPlayerInAirState(true);
    }

    if (playerState.inAir && playerState.forces.upward > -15.0) {
        RelativeVector3 gravityForce = {
            .forward = 0.0,
            .sideway = 0.0,
            .upward = -0.009 * deltaTime()
        };

        appendPlayerForces(gravityForce);
    }

    previousPlayerPositionY = position.y;
    double yPositionChange = playerState.speed * playerState.forces.upward * deltaTime() / 2.0;

    if (!collisionOnY) {
        position.y += yPositionChange;
    }

    if (collisionOnY && yPositionChange > 0.0) {
        position.y -= yPositionChange;
        RelativeVector3 forces = {
            .forward = playerState.forces.forward,
            .sideway = playerState.forces.sideway,
            .upward = -1.0
        };

        setPlayerForces(forces);
    }

    setViewportPosition(position);
}
