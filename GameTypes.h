#include <libgccvb.h>
#ifndef __GameTypes_H
#define __GameTypes_H
/**********************
Type definitions used in
the game functions
**********************/
typedef struct{
	s32 x; //fixed point
	s32 y; //fixed point
	s32 z; //fixed point
	s32 w; //fixed point
} vector3d;//represents a vector or coordinate.

typedef struct{
	vector3d world;
	vector3d rotation;
	s32 p;//parallax
	vector3d speed;//speed vector
	u8 dataSize;//number of vertices
	const s32* data;//pointer to data array
} object;

typedef struct{
	vector3d position;
	vector3d target;
	vector3d rotation;
	s32 d;
} camera;

typedef s32 matrix3d[4][4];//transformation matrix
#endif
