#include "GameData.h"
#include "GameTypes.h"

#ifndef __GameFunctions_H
#define __GameFunctions_H
/*******************************
Defines
*******************************/
#define SCREEN_HEIGHT 224
#define SCREEN_WIDTH 384
#define MAX_OBJECTS 256
#define MAX_VERTICES 256
#define EYE_Z 128
#define EYE_X 192
#define EYE_Y 112
#define _CacheEnable asm("mov 2,r15 \n ldsr r15,sr24":::"r15");
#define _CacheDisable asm("ldsr r0,sr24");
#define PARALLAX_MAX 31
#define PARALLAX_SHIFT 8
#define ROTATION_SPEED 6
/*******************************
Variables
*******************************/
//frame buffers
u32* const LFB1 = (u32*)0x00000000;
u32* const RFB1 = (u32*)0x00010000;
u32* const LFB2 = (u32*)0x00008000;
u32* const RFB2 = (u32*)0x00018000;

vector3d xProduct,rotate3d;

//graphing variables
u16 dx, dy;
s16 e, e1, e2, sx, sy, pixels, loop;

//controls
u8 dPadPressed = 0;
u8 crossHSpeed = 0;
u16 buttons;
/*******************************
Functions
*******************************/
void vbInit();
void ClearFrameBuffer(u32* buf);
void ClearAllFrameBuffers(){
	ClearFrameBuffer(LFB1);
	ClearFrameBuffer(RFB1);
	ClearFrameBuffer(LFB2);
	ClearFrameBuffer(RFB2);
}
/*********************************************************
Core Drawing Functions
*********************************************************/
void inline drawPoint(s16 x, s16 y, u32 color, s16 p);
void drawLine(vector3d v1, vector3d v2, u8 color, object* o);
void drawObject(object* o, s32 xscale, s32 yscale, s32 zscale);
/********************************************************/

/*********************************************************
3d Calculations/Functions
*********************************************************/
s32 dotProduct(vector3d* v1, vector3d* v2);
void crossProduct(vector3d* v1, vector3d* v2);
void matrix3dxVertex(vector3d* v,matrix3d m,vector3d* o);
void matrix3dxMatrix3d(matrix3d m1, matrix3d m2, matrix3d n);
void rotateMatrixX(matrix3d m, s32 angle);
void rotateMatrixY(matrix3d m, s32 angle);
void rotateMatrixZ(matrix3d m, s32 angle);
void translateMatrix(matrix3d m, s32 x, s32 y, s32 z);
void scaleMatrix(matrix3d m,s32 sx, s32 sy, s32 sz);
void copyMatrix(matrix3d m1, matrix3d m2);
/********************************************************/

/*********************************************************
System control functions
*********************************************************/
void screenControl();
void handleInput();
/********************************************************/

/*********************************************************
Game functions
*********************************************************/
void initObjects();
void moveObject(object* o, u8 d);
void moveCrossHairs();
/********************************************************/


#endif
