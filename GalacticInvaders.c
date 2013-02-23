#include "GameFunctions.h"
#include "GameData.h"
#include "GameTypes.h"

int main(){
	u8 o;
	
	vbInit();
	initObjects();
	while(1){
		handleInput();

		for(o=0; o<gameObjectsIdx; o++){
			moveObject(&gameObjects[o],7);
			drawObject(&gameObjects[o],20,20,20);
		}

		screenControl();
	}
}

void handleInput(){
	buttons = vbReadPad();
	if(K_RD & buttons){
		cam.position.z-=F_NUM_UP(20);
	}
	if(K_RU & buttons){
		cam.position.z+=F_NUM_UP(20);
	}
	if(K_LL & buttons){
		cam.position.x-=F_NUM_UP(20);
	}
	if(K_LR & buttons){
		cam.position.x+=F_NUM_UP(20);
	}
	if(K_LD & buttons){
		cam.position.y-=F_NUM_UP(20);
	}
	if(K_LU & buttons){
		cam.position.y+=F_NUM_UP(20);
	}
}

void moveObject(object* o, u8 d){
	/************************
	d is direction
	0=don't move
	1=move in x direction
	2=move in y direction
	4=move in z direction
	************************/
	o->world.x+=(d&0x01)?(o->speed.x):(0);	
	o->world.y+=(d&0x02)?(o->speed.y):(0);
	o->world.z+=(d&0x04)?(o->speed.z):(0);
}

/**************************************
Copies a matrix from m1 to m2
**************************************/
void copyMatrix(matrix3d m1, matrix3d m2){
	u8 r,c;
	
	for(r=0;r<4;r++){
		for(c=0;c<4;c++){
			m2[r][c]=m1[r][c];
		}
	}
}

void drawObject(object* o, s32 xscale, s32 yscale, s32 zscale){
	s32 size;
	vector3d v1,v2;
	vector3d vt;
	u8 verts,i;
	s32* v;
	s32* vertEnd;
	
	o->p = F_SUB(o->world.z,cam.position.z);
	
	size=o->objData.data[0];//total elements in array
	verts=o->objData.data[1];//total vertices per section
	
	v=(s32*)(o->objData.data+2);//right before first vertex
	vertEnd=(s32*)(o->objData.data+size);//last vertex
	
	worldMatrix(m_world3d,o,xscale,yscale,zscale);

	while(v < vertEnd){
		v1.x = (v)[0];
		v1.y = (++v)[0];
		v1.z = (++v)[0];
		v1.w = F_NUM_UP(1);
		
		matrix3dxVertex(&v1,m_world3d,&vt);
		
		v1.x = vt.x;
		v1.y = vt.y;
		v1.z = vt.z;

		for(i=1; i<verts; i++){			
			v2.x = (++v)[0];
			v2.y = (++v)[0];
			v2.z = (++v)[0];
			v2.w = F_NUM_UP(1);
			
			matrix3dxVertex(&v2,m_world3d,&vt);
			
			v2.x = vt.x;
			v2.y = vt.y;
			v2.z = vt.z;
			
			myDrawLine(v1,v2,3,o);
			
			v1.x = v2.x;
			v1.y = v2.y;
			v1.z = v2.z;
		}
		//This causes a final line to be drawn back to the starting vertex
		v2.x = (v-(((verts<<1) + verts)-1))[0];
		v2.y = (v-(((verts<<1) + verts)-2))[0];
		v2.z = (v-(((verts<<1) + verts)-3))[0];
		
		matrix3dxVertex(&v2,m_world3d,&vt);
		
		v2.x = vt.x;
		v2.y = vt.y;
		v2.z = vt.z;
		
		myDrawLine(v1,v2,3,o);
		
		v++;
	}
}

void initObjects(){
	cam.position.x = 0;
	cam.position.y = 0;
	cam.position.z = 0;
	cam.d = F_NUM_UP(128);
	
	//tie fighter body
	gameObjectsIdx=0;
	gameObjects[gameObjectsIdx].world.x=F_NUM_UP(-200);
	gameObjects[gameObjectsIdx].world.y=0;
	gameObjects[gameObjectsIdx].world.z=F_NUM_UP(500);
	gameObjects[gameObjectsIdx].rotation.x=0;
	gameObjects[gameObjectsIdx].rotation.y=0;
	gameObjects[gameObjectsIdx].rotation.z=0;
	gameObjects[gameObjectsIdx].p=0;
	gameObjects[gameObjectsIdx].speed.x=0;
	gameObjects[gameObjectsIdx].speed.y=0;
	gameObjects[gameObjectsIdx].speed.z=0;
	gameObjects[gameObjectsIdx].objData.data = tieFighter;
	gameObjectsIdx++;
	//tie fighter wings
	gameObjects[gameObjectsIdx].world.x=gameObjects[gameObjectsIdx-1].world.x;
	gameObjects[gameObjectsIdx].world.y=gameObjects[gameObjectsIdx-1].world.y;
	gameObjects[gameObjectsIdx].world.z=gameObjects[gameObjectsIdx-1].world.z;
	gameObjects[gameObjectsIdx].rotation.x=gameObjects[gameObjectsIdx-1].rotation.x;
	gameObjects[gameObjectsIdx].rotation.y=gameObjects[gameObjectsIdx-1].rotation.y;
	gameObjects[gameObjectsIdx].rotation.z=gameObjects[gameObjectsIdx-1].rotation.z;
	gameObjects[gameObjectsIdx].p=gameObjects[gameObjectsIdx-1].p;
	gameObjects[gameObjectsIdx].speed.x=gameObjects[gameObjectsIdx-1].speed.x;
	gameObjects[gameObjectsIdx].speed.y=gameObjects[gameObjectsIdx-1].speed.y;
	gameObjects[gameObjectsIdx].speed.z=gameObjects[gameObjectsIdx-1].speed.z;
	gameObjects[gameObjectsIdx].objData.data = tieFighterWings;
	gameObjectsIdx++;
	//tie fighter body
	gameObjects[gameObjectsIdx].world.x=F_NUM_UP(200);
	gameObjects[gameObjectsIdx].world.y=F_NUM_UP(-100);
	gameObjects[gameObjectsIdx].world.z=F_NUM_UP(500);
	gameObjects[gameObjectsIdx].rotation.x=0;
	gameObjects[gameObjectsIdx].rotation.y=0;
	gameObjects[gameObjectsIdx].rotation.z=0;
	gameObjects[gameObjectsIdx].p=0;
	gameObjects[gameObjectsIdx].speed.x=0;
	gameObjects[gameObjectsIdx].speed.y=0;
	gameObjects[gameObjectsIdx].speed.z=0;
	gameObjects[gameObjectsIdx].objData.data = tieFighter;
	gameObjectsIdx++;
	//tie fighter wings
	gameObjects[gameObjectsIdx].world.x=gameObjects[gameObjectsIdx-1].world.x;
	gameObjects[gameObjectsIdx].world.y=gameObjects[gameObjectsIdx-1].world.y;
	gameObjects[gameObjectsIdx].world.z=gameObjects[gameObjectsIdx-1].world.z;
	gameObjects[gameObjectsIdx].rotation.x=gameObjects[gameObjectsIdx-1].rotation.x;
	gameObjects[gameObjectsIdx].rotation.y=gameObjects[gameObjectsIdx-1].rotation.y;
	gameObjects[gameObjectsIdx].rotation.z=gameObjects[gameObjectsIdx-1].rotation.z;
	gameObjects[gameObjectsIdx].p=gameObjects[gameObjectsIdx-1].p;
	gameObjects[gameObjectsIdx].speed.x=gameObjects[gameObjectsIdx-1].speed.x;
	gameObjects[gameObjectsIdx].speed.y=gameObjects[gameObjectsIdx-1].speed.y;
	gameObjects[gameObjectsIdx].speed.z=gameObjects[gameObjectsIdx-1].speed.z;
	gameObjects[gameObjectsIdx].objData.data = tieFighterWings;
	gameObjectsIdx++;
}

/********************************
Matrix multiplication function
********************************/
void matrix3dxVertex(vector3d* v, matrix3d m, vector3d* o){	
	o->x = F_ADD(F_ADD(F_ADD(
	       F_MUL(v->x,m[0][0]),
	       F_MUL(v->y,m[1][0])),
	       F_MUL(v->z,m[2][0])),
	       F_MUL(v->w,m[3][0]));
	      
	o->y = F_ADD(F_ADD(F_ADD(
	       F_MUL(v->x,m[0][1]),
	       F_MUL(v->y,m[1][1])),
	       F_MUL(v->z,m[2][1])),
	       F_MUL(v->w,m[3][1]));
	      
	o->z = F_ADD(F_ADD(F_ADD(
	       F_MUL(v->x,m[0][2]),
	       F_MUL(v->y,m[1][2])),
	       F_MUL(v->z,m[2][2])),
	       F_MUL(v->w,m[3][2]));
	       
	o->w = F_ADD(F_ADD(F_ADD(
	       F_MUL(v->x,m[0][3]),
	       F_MUL(v->y,m[1][3])),
	       F_MUL(v->z,m[2][3])),
	       F_MUL(v->w,m[3][3]));
}

/************************************
Multiply matrices together to produce
single transform matrix.
************************************/
void matrix3dxMatrix3d(matrix3d m1, matrix3d m2, matrix3d n){
	u8 i,j,k;
	i=j=k=0;
	
	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			n[i][j]=0;
			for(k=0;k<4;k++){
				n[i][j]+=F_MUL(m1[i][k],m2[k][j]);
			}
		}
	}
}

/**********************************************
This is a combination of all necessary matrices.
**********************************************/
void worldMatrix(matrix3d m, object* o, s32 sx, s32 sy, s32 sz){
	s32 ax,ay,az;
	ax = F_NUM_DN((o->rotation.x<0)?(F_NUM_UP(360)+o->rotation.x):(o->rotation.x));
	ay = F_NUM_DN((o->rotation.y<0)?(F_NUM_UP(360)+o->rotation.y):(o->rotation.y));
	az = F_NUM_DN((o->rotation.z<0)?(F_NUM_UP(360)+o->rotation.z):(o->rotation.z));
	
	m[0][0]=F_MUL(F_MUL(F_NUM_UP(sx),F_COSINE(ay)),F_COSINE(az));
	m[0][1]=F_MUL(F_MUL(F_NUM_UP(sx),F_COSINE(ay)),F_SINE(az));
	m[0][2]=F_MUL(F_NUM_UP(sx),-(F_SINE(ay)));
	m[0][3]=0;
	
	m[1][0]=F_ADD(F_MUL(F_MUL(F_MUL(F_NUM_UP(sy),F_SINE(ax)),F_SINE(ay)),F_COSINE(az)),
	        F_MUL(F_MUL(F_NUM_UP(sy),F_COSINE(ax)),-F_SINE(az)));
	m[1][1]=F_ADD(F_MUL(F_MUL(F_NUM_UP(sy),F_COSINE(ax)),F_COSINE(az)),
	        F_MUL(F_MUL(F_MUL(F_NUM_UP(sy),F_SINE(ax)),F_SINE(ay)),F_SINE(az)));
	m[1][2]=F_MUL(F_MUL(F_NUM_UP(sy),F_SINE(ax)),F_COSINE(ay));
	m[1][3]=0;
	
	m[2][0]=F_ADD(F_MUL(F_MUL(F_MUL(F_NUM_UP(sz),F_COSINE(ax)),F_COSINE(az)),F_SINE(ay)),
	        F_MUL(F_MUL(F_NUM_UP(sz),-F_SINE(ax)),-F_SINE(az)));
	m[2][1]=F_ADD(F_MUL(F_MUL(F_NUM_UP(sz),-F_SINE(ax)),F_COSINE(az)),
	        F_MUL(F_MUL(F_MUL(F_NUM_UP(sz),F_COSINE(ax)),F_SINE(ay)),F_SINE(az)));
	m[2][2]=F_MUL(F_MUL(F_NUM_UP(sz),F_COSINE(ax)),F_COSINE(ay));
	m[2][3]=0;
	
	m[3][0]=F_SUB(o->world.x,cam.position.x);
	m[3][1]=F_ADD(o->world.y,cam.position.y);
	m[3][2]=F_SUB(o->world.z,cam.position.z);
	m[3][3]=F_NUM_UP(1);
}

/*********************************
Fills the camera translation matrix.
Same as regular translate except all
axis scaled by -1.
*********************************/
void translateCameraMatrix(matrix3d m, object* target){
	vector3d look, right, up;
	up.x = 0;
	up.y = F_NUM_UP(1);
	up.z = 0;
	
	right.x=F_NUM_UP(1);
	right.y=0;
	right.z=0;
	
	look.x = 0;
	look.y = 0;
	look.z = F_NUM_UP(1);
	
	m[0][0]=(right.x);
	m[0][1]=(right.y);
	m[0][2]=(right.z);
	m[0][3]=F_NUM_UP(1);
	
	m[1][0]=(up.x);
	m[1][1]=(up.y);
	m[1][1]=(up.z);
	m[1][1]=F_NUM_UP(1);
	
	m[2][0]=(look.x);
	m[2][1]=(look.y);
	m[2][2]=(look.z);
	m[2][3]=F_NUM_UP(1);
	
	m[3][0]=0;
	m[3][1]=0;
	m[3][2]=0;
	m[3][3]=F_NUM_UP(1);
	
	
}

/********************************
Fills the scale matrix
********************************/
void scaleMatrix(matrix3d m,s32 sx, s32 sy, s32 sz){
	m[0][0]=F_NUM_UP(sx);
	m[0][1]=0;
	m[0][2]=0;
	m[0][3]=0;
	
	m[1][0]=0;
	m[1][1]=F_NUM_UP(sy);
	m[1][2]=0;
	m[1][3]=0;
	
	m[2][0]=0;
	m[2][1]=0;
	m[2][2]=F_NUM_UP(sz);
	m[2][3]=0;
	
	m[3][0]=0;
	m[3][1]=0;
	m[3][2]=0;
	m[3][3]=F_NUM_UP(1);
}

void projectionMatrix(matrix3d m){
	m[0][0]=F_DIV((cam.d<<1),F_NUM_UP(SCREEN_WIDTH));
	m[0][1]=0;
	m[0][2]=0;
	m[0][3]=0;
	
	m[1][0]=0;
	m[1][1]=F_DIV((cam.d<<1),F_NUM_UP(SCREEN_HEIGHT));
	m[1][2]=0;
	m[1][3]=0;
	
	m[2][0]=0;
	m[2][1]=0;
	m[2][2]=F_DIV(F_NUM_UP(FAR_Z),F_SUB(F_NUM_UP(FAR_Z),cam.d));
	m[2][3]=F_DIV((F_MUL(cam.d,F_NUM_UP(FAR_Z))),(F_SUB(F_NUM_UP(FAR_Z),cam.d)));
	
	m[3][0]=0;
	m[3][1]=0;
	m[3][2]=F_NUM_UP(1);
	m[3][3]=0;
}

/*******************************
Fills the rotate matrix for z axis
angle is in degrees.
*******************************/
void rotateMatrixZ(matrix3d m,s32 angle){
	angle = F_NUM_DN((angle<0)?(F_NUM_UP(360)+angle):(angle));
	m[0][0]=F_COSINE(angle);
	m[0][1]=F_SINE(angle);
	m[0][2]=0;
	m[0][3]=0;
	
	m[1][0]=-(F_SINE(angle));
	m[1][1]=F_COSINE(angle);
	m[1][2]=0;
	m[1][3]=0;
	
	m[2][0]=0;
	m[2][1]=0;
	m[2][2]=F_NUM_UP(1);
	m[2][3]=0;
	
	m[3][0]=0;
	m[3][1]=0;
	m[3][2]=0;
	m[3][3]=F_NUM_UP(1);
}

/*******************************
Fills the rotate matrix for y axis
angle is in degrees.
*******************************/
void rotateMatrixY(matrix3d m,s32 angle){
	angle = F_NUM_DN((angle<0)?(F_NUM_UP(360)+angle):(angle));
	m[0][0]=F_COSINE(angle);
	m[0][1]=0;
	m[0][2]=-(F_SINE(angle));
	m[0][3]=0;
	
	m[1][0]=0;
	m[1][1]=F_NUM_UP(1);
	m[1][2]=0;
	m[1][3]=0;
	
	m[2][0]=F_SINE(angle);
	m[2][1]=0;
	m[2][2]=F_COSINE(angle);
	m[2][3]=0;
	
	m[3][0]=0;
	m[3][1]=0;
	m[3][2]=0;
	m[3][3]=F_NUM_UP(1);
}

/*******************************
Fills the rotate matrix for x axis
angle is in degrees.
*******************************/
void rotateMatrixX(matrix3d m,s32 angle){
	angle = F_NUM_DN((angle<0)?(F_NUM_UP(360)+angle):(angle));
	m[0][0]=F_NUM_UP(1);
	m[0][1]=0;
	m[0][2]=0;
	m[0][3]=0;
	
	m[1][0]=0;
	m[1][1]=F_COSINE(angle);
	m[1][2]=F_SINE(angle);
	m[1][3]=0;
	
	m[2][0]=0;
	m[2][1]=-(F_SINE(angle));
	m[2][2]=F_COSINE(angle);
	m[2][3]=0;
	
	m[3][0]=0;
	m[3][1]=0;
	m[3][2]=0;
	m[3][3]=F_NUM_UP(1);
}

/*********************************
Fills the translation matrix
*********************************/
void translateMatrix(matrix3d m, s32 x, s32 y, s32 z){
	m[0][0]=F_NUM_UP(1);
	m[0][1]=0;
	m[0][2]=0;
	m[0][3]=0;
	
	m[1][0]=0;
	m[1][1]=F_NUM_UP(1);
	m[1][2]=0;
	m[1][3]=0;
	
	m[2][0]=0;
	m[2][1]=0;
	m[2][2]=F_NUM_UP(1);
	m[2][3]=0;
	
	m[3][0]=x;
	m[3][1]=y;
	m[3][2]=z;
	m[3][3]=F_NUM_UP(1);
	
	
}

void vbInit(){
	vbDisplayOn ();
	vbSetColTable ();
	
	VIP_REGS[BRTA]  = 32;
	VIP_REGS[BRTB]  = 64;
	VIP_REGS[BRTC]  = 32;
	
	HW_REGS[WCR] = 1;
	
	VIP_REGS[FRMCYC] = 0;
}
/*******************************
Draws a pixel onto the screen
*******************************/
void inline drawPoint(s32 x, s32 y, u8 color, s32 p){
	s32 loffset,roffset;
	u8 yleft;
	
	loffset = (((x-p)<<4) + (y>>4));
	roffset = (loffset + (p<<5));
	
	if(loffset>0x5FFF || loffset<0) return;
	if(roffset>0x5FFF || roffset<0) return;
	
	color &= 0x03;
	
	yleft = (y&0x0F)<<1;

	currentFrameBuffer[loffset] |= (color<<yleft);
	((u32*)(currentFrameBuffer+0x4000))[roffset] |= (color<<yleft);
}

/*******************************
Bresenham Line Algorithm
*******************************/
void drawLine(vector3d v1, vector3d v2, u8 color, object* o){
	s32 vx,vy,vx2,vy2;
	s32 dx, dy;
	s16 e, e1, e2, sx, sy, pixels, loop;

	//z clipping(clips whole line should improve in future)
	if(v1.z<=(F_NUM_DN(cam.position.z))) return;
	if(v2.z<=(F_NUM_DN(cam.position.z))) return;
	
	//Scale everything back to integers and apply projection
	vx=F_NUM_DN(F_ADD(F_DIV(F_MUL(v1.x,cam.d),F_ADD(cam.d,v1.z)),F_NUM_UP(SCREEN_WIDTH>>1)));
	vy=F_NUM_DN(F_ADD(F_DIV(F_MUL(v1.y,cam.d),F_ADD(cam.d,v1.z)),F_NUM_UP(SCREEN_HEIGHT>>1)));

	vx2=F_NUM_DN(F_ADD(F_DIV(F_MUL(v2.x,cam.d),F_ADD(cam.d,v2.z)),F_NUM_UP(SCREEN_WIDTH>>1)));
	vy2=F_NUM_DN(F_ADD(F_DIV(F_MUL(v2.y,cam.d),F_ADD(cam.d,v2.z)),F_NUM_UP(SCREEN_HEIGHT>>1)));


	/**************************
	The following algorithm was taken from stack overflow
	http://stackoverflow.com/questions/5186939/algorithm-for-drawing-a-4-connected-line
	**************************/
	dx=vx2-vx;
	dx=((dx >> 31))?(~dx + 1):(dx); //absolute value
	dy=vy2-vy;
	dy=((dy >> 31))?(~dy + 1):(dy);
	
	sx = (vx<vx2)?(1):(-1);
	sy = (vy<vy2)?(1):(-1);
	
	pixels=dx+dy; //pixels
	e=0;
	
	CACHE_ENABLE;
	for(loop=0; loop<pixels; loop++){
		drawPoint(vx,vy,color,(F_NUM_DN(o->p)>>PARALLAX_SHIFT));
		e1=e+dy;
		e2=e-dx;
		if(((e1>>15)?(~e1 + 1):(e1)) < ((e2>>15)?(~e2 + 1):(e2))){
			vx+=sx;
			e=e1;
		}else{
			vy+=sy;
			e=e2;
		}
	}
	CACHE_DISABLE;
}

/*******************************
My Line Algorithm
*******************************/
void myDrawLine(vector3d v1, vector3d v2, u8 color, object* o){
	s32 vx,vy,vx2,vy2;
	s32 dx, dy, dmin;
	s8 shift,sy,sx,p;

	//z clipping(clips whole line should improve in future)
	if(v1.z<=(F_NUM_DN(cam.position.z))) return;
	if(v2.z<=(F_NUM_DN(cam.position.z))) return;
	
	//Scale everything back to integers and apply projection
	vx=F_NUM_DN(F_ADD(F_DIV(F_MUL(v1.x,cam.d),F_ADD(cam.d,v1.z)),F_NUM_UP(SCREEN_WIDTH>>1)));
	vy=F_NUM_DN(F_ADD(F_DIV(F_MUL(v1.y,cam.d),F_ADD(cam.d,v1.z)),F_NUM_UP(SCREEN_HEIGHT>>1)));

	vx2=F_NUM_DN(F_ADD(F_DIV(F_MUL(v2.x,cam.d),F_ADD(cam.d,v2.z)),F_NUM_UP(SCREEN_WIDTH>>1)));
	vy2=F_NUM_DN(F_ADD(F_DIV(F_MUL(v2.y,cam.d),F_ADD(cam.d,v2.z)),F_NUM_UP(SCREEN_HEIGHT>>1)));

	/****************
	Get dx and dy.
	"Normalize" dx/dy by converting to next power of 2 and shifting until dx or dy = 1;
	Calculate last line segment based on normalized dx/dy, divide by 2 and add that to the larger of dx and dy
	****************/
	dx=(~(vx-vx2))+1;
	dy=(~(vy-vy2))+1;
	
	sy=(dy<0)?(-1):(1);
	sx=(dx<0)?(-1):(1);
	
	//Get next power of 2
	if(dx<0) dx=(~dx)+1;
	if(dy<0) dy=(~dy)+1;
	
	dx = (dx ^ ~dx) + 1;
	dy = (dy ^ ~dy) + 1;
	dmin = (dx<dy)?(dx):(dy);
	
	//Get number of bits to shift to normalize
	switch(dmin){
		case(0):shift = 0;break;
		case(2):shift = 1;break;
		case(4):shift = 2;break;
		case(8):shift = 3;break;
		case(16):shift = 4;break;
		case(32):shift = 5;break;
		case(64):shift = 6;break;
		case(128):shift = 7;break;
		case(256):shift = 8;break;
		case(512):shift = 9;break;
	}
	//These are now normalized for 1 pixel
	dx = dx >> shift;
	dy = dy >> shift;
	
	//CACHE_ENABLE;
	if(dx<dy){
		while(vy!=vy2){
			for(p=0;p<dy;p++){
				drawPoint(vx,vy,3,o->p);
				vy+=sy;
				if(vy==vy2)break;
			}
			if(vx!=vx2) vx+=sx;
		}
	}else{
		while(vx!=vx2){
			for(p=0;p<dx;p++){
				drawPoint(vx,vy,3,o->p);
				vx+=sx;
				if(vx==vx2)break;
			}
			if(vy!=vy2) vy+=sy;
		}
	}
	//CACHE_DISABLE;
}

/*******************************
Dot Product function for determining
the angle between two vectors.
*******************************/
s32 dotProduct(vector3d* v1, vector3d* v2){
	s32 r;
	r = F_ADD(F_ADD(F_MUL(v1->x,v2->x),F_MUL(v1->y,v2->y)),F_MUL(v1->z,v2->z));
	return r;
}

/*******************************
Cross Product function
*******************************/
void crossProduct(vector3d* v1,vector3d* v2, vector3d* n){
	n->x = F_SUB(F_MUL(v1->y,v2->z),F_MUL(v1->z,v2->y));
	n->y = F_SUB(F_MUL(v1->z,v2->x),F_MUL(v1->x,v2->z));
	n->z = F_SUB(F_MUL(v1->x,v2->y),F_MUL(v1->y,v2->x));
}

/*******************************
Controls the timing of the screen
refresh. Borrowed from the Hunter game
written by DanB.
*******************************/
void screenControl(void){	
	u32* t;
	
	static u16 pgflip = XPBSY1;
	pgflip = ~pgflip & 0x000C;
	
	VIP_REGS[XPCTRL] = (VIP_REGS[XPSTTS] | XPEN);
	while(!(VIP_REGS[XPSTTS] & pgflip));
	VIP_REGS[XPCTRL] = (VIP_REGS[XPSTTS] & ~XPEN);//Disable drawing
	
	t = currentFrameBuffer;
	currentFrameBuffer = nextFrameBuffer;
	nextFrameBuffer = t;

}

/***********************************
This function normalizes or creates
a unit vector.
***********************************/
void normalizeVector(vector3d* v, vector3d* n){
	s32 length;
	length=isqrt(F_ADD(F_ADD(F_MUL(v->x,v->x),F_MUL(v->y,v->y)),F_MUL(v->z,v->z)));
	n->x = F_DIV(v->x,length);
	n->y = F_DIV(v->y,length);
	n->z = F_DIV(v->z,length);
}

/***********************************
Copy drawing data into frame buffers
***********************************/
void copyBuffer(u32* buffer, u32* fBuffer, u32 buffSize){
	u32* p1 = fBuffer;
	u32* p2 = buffer;
	u32* ps = p1 + buffSize;
	while(p1<ps){
		p1[0]=p2[0];
		p1[1]=p2[1];
		p1[2]=p2[2];
		p1[3]=p2[3];
		p1[4]=p2[4];
		p1[5]=p2[5];
		p1[6]=p2[6];
		p1[7]=p2[7];
		p1+=8;
		p2+=8;
	}
}

/*************************************
Square root function based on integers
This came from wikipedia
*************************************/
s32 isqrt(s32 num) {
    s32 res = 0;
    s32 bit = 1 << 30; // The second-to-top bit is set: 1L<<30 for long
 
    // "bit" starts at the highest power of four <= the argument.
    while (bit > num)
        bit >>= 2;
 
    while (bit != 0) {
        if (num >= res + bit) {
            num -= res + bit;
            res = (res >> 1) + bit;
        }
        else res >>= 1;
        
        bit >>= 2;
    }
    return res;
}
