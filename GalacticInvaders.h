#include <libgccvb.h>

#ifndef __GalacticInvaders_H
#define __GalacticInvaders_H
/*******************************
Defines
*******************************/
//object array header
#define WX 0
#define WY 1
#define WZ 2
#define SX 3
#define SY 4
#define SZ 5
#define RX 6
#define RY 7
#define RZ 8
#define PARALLAX 9
#define POLYSTART 10 //give number of defined vertices for polygon

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
/*******************************
Variables
*******************************/
//frame buffers
u32* const LFB1 = (u32*)0x00000000;
u32* const RFB1 = (u32*)0x00010000;
u32* const LFB2 = (u32*)0x00008000;
u32* const RFB2 = (u32*)0x00018000;

s32 xProduct[3];

//graphing variables
u16 dx, dy;
s16 e, e1, e2, sx, sy, pixels, loop;
//u8 yleft;

//controls
u8 dPadPressed = 0;
u8 crossHSpeed = 0;
u16 buttons;
/*******************************
Functions
*******************************/
void vbInit(){
	vbDisplayOn ();
	vbSetColTable ();
	
	VIP_REGS[BRTA]  = 32;
	VIP_REGS[BRTB]  = 64;
	VIP_REGS[BRTC]  = 32;
	
	HW_REGS[WCR] = 1;
}

void ClearFrameBuffer(u32* buf){
	u16 i;
	for(i=0;i<0x2000;i++){
		buf[i] = 0x00000000;
	}
}

void ClearAllFrameBuffers(){
	ClearFrameBuffer(LFB1);
	ClearFrameBuffer(RFB1);
	ClearFrameBuffer(LFB2);
	ClearFrameBuffer(RFB2);
}

/*******************************
Draws a pixel onto the screen
*******************************/
void inline drawPoint(s16 x, s16 y, u32 color, s16 p){
	/*
	loffset = ((x-p)<<4) + (y>>4);
	roffset = ((x+p)<<4) + (y>>4);
	color &= 0x03;
	
	yleft = y&0x0F;
	if(yleft>0) yleft <<= 1;
	
	color <<= yleft;
	
	LFB1[loffset] |= color;
	RFB1[roffset] |= color;
	LFB2[loffset] |= color;
	RFB2[roffset] |= color;
	*/
	asm(
	"mov %0,r5\n" //store x
	"mov %1,r6\n" //store y
	"mov %2,r8\n" //store color
	"mov %3,r9\n" //store p
	//loffset
	"mov r5,r10\n"
	"sub r9,r10\n" //x-p
	"shl 4,r10\n" //(x-p)<<4
	"mov r6,r11\n" 
	"shr 4,r11\n" //y>>4
	"add r11,r10\n" //((x-p)<<4)+(y>>4)
	"st.w r10,-4[r3]\n" //store to stack
	//roffset
	"mov r5,r10\n"
	"add r9,r10\n" //(x+p)
	"shl 4,r10\n" //(x+p)<<4
	"mov r6,r11\n"
	"shr 4,r11\n" //y>>4
	"add r11,r10\n" //((x+p)+(y>>4)
	"st.w r10,-8[r3]\n" //store to stack
	//color
	"andi 0x03,r8,r8\n" //color &= 0x03
	//yleft
	"mov r6,r10\n"
	"andi 0x0F,r10,r10\n" //y&0x0F
	"ble _next\n" //if(yleft>0)
	"shl 1,r10\n" //yleft <<= 1
	"_next:\n"
	//(color << yleft)
	"shl r10,r8\n" //shift left by yleft
	//LFB1[loffset] |= (color<<yleft)
	"mov %4,r11\n" //LFB1
	"ld.w -4[r3],r12\n" //loffset
	"shl 2,r12\n" //loffset * 4 bytes
	"add r12,r11\n" //LFB1[loffset]
	"ld.w 0[r11],r12\n"
	"or r8,r12\n" //LFB1[loffset]|=(color<<yleft)
	"st.w r12,0[r11]\n"
	//RFB1[roffset] |= (color<<yleft)
	"mov %5,r11\n" //RFB1
	"ld.w -8[r3],r12\n" //roffset
	"shl 2,r12\n" //roffset * 4bytes
	"add r12,r11\n" //RFB1[roffset]
	"ld.w 0[r11],r12\n"
	"or r8,r12\n" //RFB1[roffset]|=(color<<yleft)
	"st.w r12,0[r11]\n"
	//LFB2[loffset] |= (color<<yleft)
	"mov %6,r11\n" //LFB2
	"ld.w -4[r3],r12\n" //loffset
	"shl 2,r12\n" //loffset * 4 bytes
	"add r12,r11\n" //LFB2[loffset]
	"ld.w 0[r11],r12\n"
	"or r8,r12\n" //LFB2[loffset]|=(color<<yleft)
	"st.w r12,0[r11]\n"
	//RFB2[roffset] |= (color<<yleft)
	"mov %7,r11\n" //RFB2
	"ld.w -8[r3],r12\n" //roffset
	"shl 2,r12\n" //roffset * 4 bytes
	"add r12,r11\n" //RFB2[roffset]
	"ld.w 0[r11],r12\n"
	"or r8,r12\n" //RFB2[loffset]|=(color<<yleft)
	"st.w r12,0[r11]\n"
		:
		:"r" (x),"r" (y),"r" (color),
		 "r" (p),"r" (LFB1),"r" (RFB1),"r" (LFB2),
		 "r" (RFB2)
		:"r5","r6","r8","r9","r10","r11","r12"
		);
	
}

/*******************************
Bresenham Line Algorithm
*******************************/
void drawLine(s16 x1,s16 y1,s16 z1,s16 x2,s16 y2, s16 z2, u8 color, s16 p){
	if(z1<EYE_Z || z2<EYE_Z) return;
	
	//Adjust values
	/*
	x1 = ((x1-EYE_X)*EYE_Z)/(EYE_Z+z1) + EYE_X;
	x2 = ((x2-EYE_X)*EYE_Z)/(EYE_Z+z2) + EYE_X;
	y1 = ((y1-EYE_Y)*EYE_Z)/(EYE_Z+z1) + EYE_Y;
	y2 = ((y2-EYE_Y)*EYE_Z)/(EYE_Z+z2) + EYE_Y;
	*/
	//x1 and x2
	asm(
		"mov %2,r5\n" //EYE_X
		"mov %3,r6\n" //EYE_Z
		"mov %4,r7\n" //x1
		"mov %5,r8\n" //z1
		"mov %6,r9\n" //x2
		"mov %7,r10\n" //z2
		
		"sub r5,r7\n" //x1-EYE_X
		"sub r5,r9\n" //x2-EYE_X
		"add r6,r8\n" //EYE_Z+z1
		"add r6,r10\n" //EYE_Z+z2
		
		"mul r6,r7\n" //(x1-EYE_X)*EYE_Z
		"div r8,r7\n" // /(EYE_Z+z1)
		"add r5,r7\n" // +EYE_X
		"mov r7,%0\n"
		
		"mul r6,r9\n" //(x2-EYE_X)*EYE_Z
		"div r10,r9\n" // /(EYE_Z+z2)
		"add r5,r9\n" // +EYE_X
		"mov r9,%1\n"
		
		:"=r"(x1),"=r"(x2)
		:"r"(EYE_X),"r"(EYE_Z),"r"(x1),
		 "r"(z1),"r"(x2),"r"(z2)
	);
	//y1 and y2
	asm(
		"mov %2,r5\n" //EYE_Y
		"mov %3,r6\n" //EYE_Z
		"mov %4,r7\n" //y1
		"mov %5,r8\n" //z1
		"mov %6,r9\n" //y2
		"mov %7,r10\n" //z2
		
		"sub r5,r7\n" //y1-EYE_Y
		"sub r5,r9\n" //y2-EYE_Y
		"add r6,r8\n" //EYE_Z+z1
		"add r6,r10\n" //EYE_Z+z2
		
		"mul r6,r7\n" //(y1-EYE_Y)*EYE_Z
		"div r8,r7\n" // /(EYE_Z+z1)
		"add r5,r7\n" // +EYE_Y
		"mov r7,%0\n"
		
		"mul r6,r9\n" //(y2-EYE_Y)*EYE_Z
		"div r10,r9\n" // /(EYE_Z+z2)
		"add r5,r9\n" // +EYE_Y
		"mov r9,%1\n"
		
		:"=r"(y1),"=r"(y2)
		:"r"(EYE_Y),"r"(EYE_Z),"r"(y1),
		 "r"(z1),"r"(y2),"r"(z2)
	);
	/*
	if(x1<(EYE_X-(SCREEN_WIDTH>>1)) || x1>(EYE_X+(SCREEN_WIDTH>>1))) return;
	if(y1<(EYE_Y-(SCREEN_HEIGHT>>1)) || y1>(EYE_Y+(SCREEN_HEIGHT>>1))) return;
	if(x2<(EYE_X-(SCREEN_WIDTH>>1)) || x2>(EYE_X+(SCREEN_WIDTH>>1))) return;
	if(y2<(EYE_Y-(SCREEN_HEIGHT>>1)) || y2>(EYE_Y+(SCREEN_HEIGHT>>1))) return;
	*/
	/**************************
	The following algorithm was taken from stack overflow
	http://stackoverflow.com/questions/5186939/algorithm-for-drawing-a-4-connected-line
	**************************/
	//dx=abs(x2-x1);
	//dy=abs(y2-y1);
	asm(
		"mov %2,r5\n" //x1
		"mov %3,r6\n" //x2
		"mov %4,r7\n" //y1
		"mov %5,r8\n" //y2
		"sub r6,r5\n"
		"bge _next1\n"
		"not r5,r5\n"
		"add 1,r5\n"
		"_next1:\n"
		"mov r5,%0\n"
		"sub r8,r7\n"
		"bge _next2\n"
		"not r7,r7\n"
		"add 1,r7\n"
		"_next2:\n"
		"mov r7,%1\n"
		:"=r"(dx),"=r"(dy)
		:"r"(x1),"r"(x2),"r"(y1),"r"(y2)
		:"r5","r6","r7","r8"
		);
	
	//sx = (x1<x2)?(1):(-1);
	//sy = (y1<y2)?(1):(-1);
	
	asm(
		"mov %2,r5\n" //x1
		"mov %3,r6\n" //x2
		"mov %4,r7\n" //y1
		"mov %5,r8\n" //y2
		"mov 1 ,r9\n"
		"cmp r5,r6\n"
		"bge _next3\n"
		"mov -1,r9\n"
		"_next3:\n"
		"mov r9,%0\n"
		"mov 1,r9\n"
		"cmp r7,r8\n"
		"bge _next4\n"
		"mov -1,r9\n"
		"_next4:\n"
		"mov r9,%1\n"
		:"=r"(sx),"=r"(sy)
		:"r"(x1),"r"(x2),"r"(y1),"r"(y2)
		:"r5","r6","r7","r8","r9"
		);
	
	//pixels=dx+dy; //pixels
	//e=0;
	
	asm(
		"mov %2,r5\n" //dx
		"mov %3,r6\n" //dy
		"add r5,r6\n"
		"mov r6,%0\n"
		"mov 0,%1\n"
		:"=r"(pixels),"=r"(e)
		:"r"(dx),"r"(dy)
		:"r5","r6"
		);
	
	for(loop=0; loop<pixels; loop++){
		drawPoint(x1,y1,color,p);
		//e1=e+dy;
		//e2=e-dx;
		asm(
			"mov %1,r5\n"
			"mov %2,r6\n"
			"add r5,r6\n"
			"mov r6,%0\n"
			:"=r"(e1)
			:"r"(e),"r"(dy)
			:"r5","r6"
			);
		asm(
			"mov %1,r5\n"
			"mov %2,r6\n"
			"sub r6,r5\n"
			"mov r5,%0\n"
			:"=r"(e2)
			:"r"(e),"r"(dx)
			:"r5","r6"
			);
		if(abs(e1)<abs(e2)){
			//x1+=sx;
			//e=e1;
			asm(
				"mov %2,r5\n" //x1
				"mov %3,r6\n" //sx
				"mov %4,%1\n" //e1
				"add r6,r5\n"
				"mov r5,%0\n"
				:"=r"(x1),"=r"(e)
				:"r"(x1),"r"(sx),"r"(e1)
				:"r5","r6"
				);
		}else{
			//y1+=sy;
			//e=e2;
			asm(
				"mov %2,r5\n" //y1
				"mov %3,r6\n" //sy
				"mov %4,%1\n" //e2
				"add r6,r5\n"
				"mov r5,%0\n"
				:"=r"(y1),"=r"(e)
				:"r"(y1),"r"(sy),"r"(e2)
				:"r5","r6"
				);
		}
	}
}

/*******************************
Dot Product function for determining
the angle between two vectors.
Used for backface culling.
*******************************/
s32 dotProduct(s32 x1, s32 y1, s32 z1, s32 x2, s32 y2, s32 z2){
	s32 r;
	//r = (x1*x2)+(y1*y2)+(z1*z2);
	asm(
		"mov %1,r5\n"//x1
		"mov %2,r6\n"//y1
		"mov %3,r7\n"//z1
		"mov %4,r8\n"//x2
		"mov %5,r9\n"//y2
		"mov %6,r10\n"//z2
		"mul r8,r5\n"//(x1*x2)
		"mul r9,r6\n"//(y1*y2)
		"mul r10,r7\n"//(z1*z2)
		"add r6,r5\n"//(x1*x2)+(y1*y2)
		"add r7,r5\n"//(x1*x2)+(y1*y2)+(z1*z2)
		"mov r5,%0\n"
		:"=r" (r)
		:"r" (x1),"r" (y1),"r" (z1),
		 "r" (x2),"r" (y2),"r" (z2)
		:"r5","r6","r7","r8","r9","r10"
		);
	return r;
}
/*******************************
Cross Product function for determining
the normal of a polygon.
Sets the values of xProduct[x,y,z]
*******************************/
void crossProduct(s32 x1,s32 y1,s32 z1,s32 x2,s32 y2,s32 z2){
	//xProduct[0] = (y1*z2)-(z1*y2);
	//xProduct[1] = (z1*x2)-(x1*z2);
	//xProduct[2] = (x1*y2)-(y1*x2);
	asm(
		"mov %3,r5\n"//x1
		"mov %4,r6\n"//y1
		"mov %5,r7\n"//z1
		"mov %6,r8\n"//x2
		"mov %7,r9\n"//y2
		"mov %8,r10\n"//z2
		"mov r6,r12\n"//r12 = xProduct[0]
		"mul r10,r12\n"//y1*z2
		"mov r7,r11\n"//r11=scratch reg
		"mul r9,r11\n"//(z1*y2)
		"sub r11,r12\n"//(y1*z2)-(z1*y2)
		"mov r7,r13\n"//r13 = xProduct[1]
		"mul r8,r13\n"//z1*x2
		"mov r5,r11\n"//x1
		"mul r10,r11\n"//x1*z2
		"sub r11,r13\n"//(z1*x2)-(x1*z2)
		"mov r5,r14\n"//r14 = xProduct[2]
		"mul r9,r14\n"//(x1*y2)
		"mov r6,r11\n"//y1
		"mul r8,r11\n"//(y1*x2)
		"sub r11,r14\n"//(x1*y2)-(y1*x2)
		:"=r"(xProduct[0]),"=r"(xProduct[1]),
		 "=r"(xProduct[1])
		:"r"(x1),"r"(y1),"r"(z1),
		 "r"(x2),"r"(y2),"r"(z2)
		:"r5","r6","r7","r8","r9","r10",
		 "r11","r12","r13","r14"
		);
}
/*******************************
Controls the timing of the screen
refresh. Borrowed from the Hunter game
written by DanB.
*******************************/
void screenControl(){
	
	static u16 pgflip = XPBSY1;
	pgflip = ~pgflip & XPBSYR;
	
	VIP_REGS[XPCTRL] = VIP_REGS[XPSTTS] | XPEN;
	while(!(VIP_REGS[XPSTTS] & pgflip));
	VIP_REGS[XPCTRL] = VIP_REGS[XPSTTS] & XPENOFF;//Disable drawing
	
}

void initObjects();
void drawObject(s32* o, s32 size, s32 xscale, s32 yscale, s32 zscale);
void moveObject(s32* o, u8 d);
void drawBuildings();
void moveCrossHairs();

#endif
