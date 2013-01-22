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

#define OBJDATA_T_PRIM_8 1 //(1/8) object in  +x,+y,+z coordinates
#define OBJDATA_T_PRIM_4 2 //(1/4) object in  +x,+y,+z and +x,-y,+z 
#define OBJDATA_T_PRIM_2 3 //(1/2) object in  +x,+y,+-z and +x,-y,+-z
#define OBJDATA_T_NORMAL 4 //No symetry. Just draw all vertices.
typedef struct{
	u8 type;//OBJDATA_T_XXX
	u8 size;//number of vertices
	const s32* data;//pointer to data
} objectData;

typedef struct{
	u8 visible;
} objectAttributes;

typedef struct object{
	struct object* parent;
	struct object* sibling;
	struct object* child;
	
	objectAttributes attributes;
	vector3d world;
	vector3d rotation;
	vector3d speed;
	s32 p;//parallax
	objectData objData;
} object;

typedef struct{
	vector3d position;
	vector3d target;
	vector3d rotation;
	s32 d;
} camera;

typedef s32 matrix3d[4][4];//transformation matrix
#endif
