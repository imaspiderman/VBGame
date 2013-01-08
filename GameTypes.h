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
	s32 wx;//world x
	s32 wy;//world y
	s32 wz;//world z
	s32 rx;//rotation x axis
	s32 ry;//rotation y axis
	s32 rz;//rotation z axis
	s32 p;//parallax
	s32 c_min_x;//clipping distance to min x
	s32 c_max_x;//clipping distance to max x
	s32 c_min_y;//clipping distance to min y
	s32 c_max_y;//clipping distance to max y
	s32 c_min_z;//clipping distance to min z
	s32 c_max_z;//clipping distance to max z
	vector3d speed;//speed vector
	u8 dataSize;//number of vertices
	const s32* data;//pointer to data array
} object;

typedef struct{
	vector3d position;
	vector3d target;
	s32 rx;
	s32 ry;
	s32 rz;
} camera;

typedef s32 matrix3d[4][4];//transformation matrix
#endif
