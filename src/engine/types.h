#ifndef BLOCKS_TYPES
#define BLOCKS_TYPES

typedef struct Vector3 {
	double x;
	double y;
	double z;
} Vector3;

typedef struct RelativeVector3 {
	double forward;
	double upward;
	double sideway;
} RelativeVector3;

typedef struct Vector2 {
	double x;
	double y;
} Vector2;

typedef struct RelativeVector2 {
	double forward;
	double upward;
};

#endif