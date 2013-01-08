#include <libgccvb.h>
#ifndef __GameTypes_H
#define __GameTypes_H
/**********************
Type definitions used in
the game functions
**********************/
typedef struct{
	s32 x;
	s32 y;
	s32 z;
} vector3d;//represents a vector or coordinate.

typedef struct{
	vector3d v[3];
} polygon;//represents a full triangle

typedef struct{
	vector3d world;//world x,y,z
	vector3d rotation;//rotation (degrees) x,y,z
	vector3d min_clip;
	vector3d max_clip;
	vector3d speed;//speed vector
	s32 p;//parallax
	u8 dataSize;//number of vertices
	const s32* data;//pointer to data array
} object;

typedef struct{
	vector3d position;
	vector3d target;
	vector3d rotation;
} camera;

typedef s32 matrix3d[4][4];//transformation matrix [rows][columns]
#endif
