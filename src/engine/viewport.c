#include "viewport.h"

Vector3 currentViewportPosition = {
	.x = 0.5,
	.y = 1.0,
	.z = 0.5
};

Vector3 currentViewportRotation = {
	.x = 1.0,
	.y = 0.0,
	.z = 0.0
};

Vector3 getViewportPosition() {
	return currentViewportPosition;
}

void setViewportPosition(Vector3 position) {
	currentViewportPosition = position;
}

Vector3 getViewportRotation() {
	return currentViewportRotation;
}

void setViewportRotation(Vector3 rotation) {
	currentViewportRotation = rotation;
}
