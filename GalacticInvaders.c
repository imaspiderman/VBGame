#include "GameFunctions.h"
#include "GameData.h"
#include "GameTypes.h"

int count=0;

int main(){
	ClearAllFrameBuffers();
	vbInit();
	initObjects();
	//Setup Timer
	HW_REGS[TCR]|=(Z_Stat_Clr);
	HW_REGS[TLR]=10;
	HW_REGS[THR]=0;
	HW_REGS[TCR]|=(T_Enb);
	
	WORLD_SET_HEAD(31,WRLD_END);
	
	while(1){
		/*Timer code
		if(HW_REGS[TCR]&Z_Stat){
			count++;
			if(count >= 10){
				VIP_REGS[BKCOL]=~(u16)VIP_REGS[BKCOL];
				HW_REGS[TCR]|=(Z_Stat_Clr);
				HW_REGS[TLR]=10;
				HW_REGS[THR]=0;
				HW_REGS[TCR]|=(T_Enb);
				count=0;
			}
		}
		*/
		moveObject(starFoxShip,15);
		handleInput();
		drawObject(starFoxShip,2,2,2);
		screenControl();
	}
}
/*
void moveCrossHairs(){
	s32 currspeed;
	buttons = vbReadPad();
	if(K_LPAD & buttons) crossHSpeed += 2;
	else {
		crossHSpeed = 0;
		crossHairs->speed.x=0;
		crossHairs->speed.y=0;
		crossHairs->speed.z=0;
	}
	
	if(K_LL & buttons){
		crossHairs->speed.x+=crossHSpeed*-1;
		moveObject(crossHairs, 9);
	}
	if(K_LR & buttons){
		crossHairs->speed.x+=crossHSpeed;
		moveObject(crossHairs, 9);
	}
	if(K_LU & buttons){
		crossHairs->speed.y+=crossHSpeed*-1;
		moveObject(crossHairs, 10);
	}
	if(K_LD & buttons){
		crossHairs->speed.y+=crossHSpeed;
		moveObject(crossHairs, 10);
	}
}
*/

void handleInput(){
	buttons = vbReadPad();
	if(K_LL & buttons){
		starFoxShip->rotation.z-=ROTATION_SPEED;
		if(starFoxShip->rotation.z<=-F_NUM_UP(360) || starFoxShip->rotation.z>=F_NUM_UP(360)) starFoxShip->rotation.z=0;
	}
	if(K_LR & buttons){
		starFoxShip->rotation.z+=ROTATION_SPEED;
		if(starFoxShip->rotation.z>=F_NUM_UP(360) || starFoxShip->rotation.z<=-F_NUM_UP(360)) starFoxShip->rotation.z=0;
	}
	if(K_LU & buttons){
		starFoxShip->rotation.x-=ROTATION_SPEED;
		if(starFoxShip->rotation.x<=-F_NUM_UP(360) || starFoxShip->rotation.x>=F_NUM_UP(360)) starFoxShip->rotation.x=0;
	}
	if(K_LD & buttons){
		starFoxShip->rotation.x+=ROTATION_SPEED;
		if(starFoxShip->rotation.x>=F_NUM_UP(360) || starFoxShip->rotation.x<=-F_NUM_UP(360)) starFoxShip->rotation.x=0;
	}
	if(K_RL & buttons){
		starFoxShip->rotation.y-=ROTATION_SPEED;
		if(starFoxShip->rotation.y<=-F_NUM_UP(360) || starFoxShip->rotation.y>=F_NUM_UP(360)) starFoxShip->rotation.y=0;
	}
	if(K_RR & buttons){
		starFoxShip->rotation.y+=ROTATION_SPEED;
		if(starFoxShip->rotation.y>=F_NUM_UP(360) || starFoxShip->rotation.y<=-F_NUM_UP(360)) starFoxShip->rotation.y=0;
	}
}

void moveObject(object* o, u8 d){
	/************************
	d is direction
	0=don't move
	1=move in x direction
	2=move in y direction
	4=move in z direction
	8=constrain to screen
	************************/
	o->world.x+=(d&0x01)?(o->speed.x):(0);	
	o->world.y+=(d&0x02)?(o->speed.y):(0);
	o->world.z+=(d&0x04)?(o->speed.z):(0);
	
	/*
	if(d&0x08){
		s32 x = ((o->world.x-EYE_X)*EYE_Z)/(EYE_Z+o->world.z)+EYE_X;
		s32 y = ((o->world.y-EYE_Y)*EYE_Z)/(EYE_Z+o->world.z)+EYE_Y;
		
		if(x<0 && d&0x01)  o->world.x=sLeft;
		if(x>SCREEN_WIDTH && d&0x01) o->world.x=sRight;
		if(y<0 && d&0x02) o->world.y=sTop;
		if(y>SCREEN_HEIGHT && d&0x02) o->world.y=sBottom;
	}
	*/
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
	s32 i,loop,p,dotp;
	vector3d v1,v2,v3;
	vector3d vt;
	matrix3d m_temp;
	o->p = ((F_NUM_DN(o->world.z)-cam.d)>>PARALLAX_SHIFT);
	
	//translateCameraMatrix(m_camera3d, o);
	worldMatrix(m_world3d,o,xscale,yscale,zscale);
	projectionMatrix(m_project3d);
	
	//matrix3dxMatrix3d(m_camera3d,m_world3d,m_temp);
	//copyMatrix(m_temp,m_world3d);
	matrix3dxMatrix3d(m_world3d,m_project3d,m_temp);
	copyMatrix(m_temp,m_world3d);
	
	//translateMatrix(m_translate3d, o->world.x, o->world.y, o->world.z);
	//rotateMatrixX(m_rotate3d_x,o->rotation.x);
	//rotateMatrixY(m_rotate3d_y,o->rotation.y);
	//rotateMatrixZ(m_rotate3d_z,o->rotation.z);
	//scaleMatrix(m_scale3d,xscale,yscale,zscale);
	
	//matrix3dxMatrix3d(m_scale3d,m_rotate3d_x,m_temp);
	//copyMatrix(m_temp,m_world3d);
	//matrix3dxMatrix3d(m_world3d,m_rotate3d_y,m_temp);
	//copyMatrix(m_temp,m_world3d);
	//matrix3dxMatrix3d(m_world3d,m_rotate3d_z,m_temp);
	//copyMatrix(m_temp,m_world3d);
	//matrix3dxMatrix3d(m_world3d,m_translate3d,m_temp);
	//copyMatrix(m_temp,m_world3d);
	//matrix3dxMatrix3d(m_world3d,m_project3d,m_temp);
	//copyMatrix(m_temp,m_world3d);
	
	
	_CacheEnable
	i=0;
	while(i<o->dataSize){
		v1.x = F_NUM_UP(o->data[i]);
		v1.y = F_NUM_UP(o->data[i+1]);
		v1.z = F_NUM_UP(o->data[i+2]);
		v1.w = F_NUM_UP(1);
		
		matrix3dxVertex(&v1,m_world3d,&vt);
		
		v1.x = vt.x;
		v1.y = vt.y;
		v1.z = vt.z;
		
		v2.x = F_NUM_UP(o->data[i+3]);
		v2.y = F_NUM_UP(o->data[i+4]);
		v2.z = F_NUM_UP(o->data[i+5]);
		v2.w = F_NUM_UP(1);
		
		matrix3dxVertex(&v2,m_world3d,&vt);
		
		v2.x = vt.x;
		v2.y = vt.y;
		v2.z = vt.z;
		
		v3.x = F_NUM_UP(o->data[i+6]);
		v3.y = F_NUM_UP(o->data[i+7]);
		v3.z = F_NUM_UP(o->data[i+8]);
		v3.w = F_NUM_UP(1);
		
		matrix3dxVertex(&v3,m_world3d,&vt);
		
		v3.x = vt.x;
		v3.y = vt.y;
		v3.z = vt.z;

		drawLine(v1,v2,3,o);
		drawLine(v2,v3,3,o);
		drawLine(v3,v1,3,o);
		i+=9;
		
	}
	_CacheDisable
}

void initObjects(){
	cam.position.x = 0;
	cam.position.y = 0;
	cam.position.z = 0;
	cam.d = F_NUM_UP(128);
	
	gameObjectsIdx=0;
	gameObjects[gameObjectsIdx].world.x=0;
	gameObjects[gameObjectsIdx].world.y=0;
	gameObjects[gameObjectsIdx].world.z=F_NUM_UP(100);
	gameObjects[gameObjectsIdx].rotation.x=0;
	gameObjects[gameObjectsIdx].rotation.y=0;
	gameObjects[gameObjectsIdx].rotation.z=0;
	gameObjects[gameObjectsIdx].p=0;
	gameObjects[gameObjectsIdx].speed.x=0;
	gameObjects[gameObjectsIdx].speed.y=0;
	gameObjects[gameObjectsIdx].speed.z=0;
	gameObjects[gameObjectsIdx].dataSize=STARFOXSHIPDATASIZE;
	gameObjects[gameObjectsIdx].data = starFoxShipData;
	starFoxShip = &gameObjects[gameObjectsIdx];
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
	s32 t;
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
	
	m[0][0]=F_MUL(F_MUL(F_NUM_UP(sx),cosine[ay]),cosine[az]);
	m[0][1]=F_MUL(F_MUL(F_NUM_UP(sx),cosine[ay]),sine[az]);
	m[0][2]=F_MUL(F_NUM_UP(sx),-(sine[ay]));
	m[0][3]=0;
	
	m[1][0]=F_ADD(F_MUL(F_MUL(F_MUL(F_NUM_UP(sy),sine[ax]),sine[ay]),cosine[az]),
	        F_MUL(F_MUL(F_NUM_UP(sy),cosine[ax]),-sine[az]));
	m[1][1]=F_ADD(F_MUL(F_MUL(F_NUM_UP(sy),cosine[ax]),cosine[az]),
	        F_MUL(F_MUL(F_MUL(F_NUM_UP(sy),sine[ax]),sine[ay]),sine[az]));
	m[1][2]=F_MUL(F_MUL(F_NUM_UP(sy),sine[ax]),cosine[ay]);
	m[1][3]=0;
	
	m[2][0]=F_ADD(F_MUL(F_MUL(F_MUL(F_NUM_UP(sz),cosine[ax]),cosine[az]),sine[ay]),
	        F_MUL(F_MUL(F_NUM_UP(sz),-sine[ax]),-sine[az]));
	m[2][1]=F_ADD(F_MUL(F_MUL(F_NUM_UP(sz),-sine[ax]),cosine[az]),
	        F_MUL(F_MUL(F_MUL(F_NUM_UP(sz),cosine[ax]),sine[ay]),sine[az]));
	m[2][2]=F_MUL(F_MUL(F_NUM_UP(sz),cosine[ax]),cosine[ay]);
	m[2][3]=0;
	
	m[3][0]=F_NUM_UP(SCREEN_WIDTH>>1);
	m[3][1]=F_NUM_UP(SCREEN_HEIGHT>>1);
	m[3][2]=0;
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
	m[2][3]=F_NUM_UP(1);
	
	m[3][0]=0;
	m[3][1]=0;
	m[3][2]=F_DIV(F_MUL(cam.d,F_NUM_UP(FAR_Z)),(F_SUB(cam.d,F_NUM_UP(FAR_Z))));
	m[3][3]=0;
}

/*******************************
Fills the rotate matrix for z axis
angle is in degrees.
*******************************/
void rotateMatrixZ(matrix3d m,s32 angle){
	angle = F_NUM_DN((angle<0)?(F_NUM_UP(360)+angle):(angle));
	m[0][0]=cosine[angle];
	m[0][1]=sine[angle];
	m[0][2]=0;
	m[0][3]=0;
	
	m[1][0]=-(sine[angle]);
	m[1][1]=cosine[angle];
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
	m[0][0]=cosine[angle];
	m[0][1]=0;
	m[0][2]=-(sine[angle]);
	m[0][3]=0;
	
	m[1][0]=0;
	m[1][1]=F_NUM_UP(1);
	m[1][2]=0;
	m[1][3]=0;
	
	m[2][0]=sine[angle];
	m[2][1]=0;
	m[2][2]=cosine[angle];
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
	m[1][1]=cosine[angle];
	m[1][2]=sine[angle];
	m[1][3]=0;
	
	m[2][0]=0;
	m[2][1]=-(sine[angle]);
	m[2][2]=cosine[angle];
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
}

void ClearFrameBuffer(u32* buf){
	u16 i;
	for(i=0;i<0x2000;i++){
		buf[i] = 0x00000000;
	}
}

/*******************************
Draws a pixel onto the screen
*******************************/
void inline drawPoint(s16 x, s16 y, u32 color, s16 p){
	s32 loffset,roffset;
	u8 yleft;
	
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
	
}

/*******************************
Bresenham Line Algorithm
*******************************/
void drawLine(vector3d v1, vector3d v2, u8 color, object* o){
	s32 vx,vy,vz,vx2,vy2,vz2;
	//Scale everything back to integers
	vx=F_NUM_DN(F_DIV(v1.x,v1.w));
	vy=F_NUM_DN(F_DIV(v1.y,v1.w));
	vz=F_NUM_DN(v1.z);
	vx2=F_NUM_DN(F_DIV(v2.x,v2.w));
	vy2=F_NUM_DN(F_DIV(v2.y,v2.w));
	vz2=F_NUM_DN(v2.z);
	
	//Adjust values
	/*
	vx = (((v1.x-EYE_X)*EYE_Z)/(EYE_Z+v1.z) + EYE_X)+(SCREEN_WIDTH>>1);
	vx2 = (((v2.x-EYE_X)*EYE_Z)/(EYE_Z+v2.z) + EYE_X)+(SCREEN_WIDTH>>1);
	vy = (((v1.y-EYE_Y)*EYE_Z)/(EYE_Z+v1.z) + EYE_Y)+(SCREEN_HEIGHT>>1);
	vy2 = (((v2.y-EYE_Y)*EYE_Z)/(EYE_Z+v2.z) + EYE_Y)+(SCREEN_HEIGHT>>1);
	*/
	/**************************
	The following algorithm was taken from stack overflow
	http://stackoverflow.com/questions/5186939/algorithm-for-drawing-a-4-connected-line
	**************************/
	dx=abs(vx2-vx);
	dy=abs(vy2-vy);
	
	sx = (vx<vx2)?(1):(-1);
	sy = (vy<vy2)?(1):(-1);
	
	pixels=dx+dy; //pixels
	e=0;
	
	for(loop=0; loop<pixels; loop++){
		drawPoint(vx,vy,color,F_NUM_DN(o->p));
		e1=e+dy;
		e2=e-dx;
		if(abs(e1)<abs(e2)){
			vx+=sx;
			e=e1;
		}else{
			vy+=sy;
			e=e2;
		}
	}
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
void screenControl(){
	static u16 pgflip = XPBSY1;
	pgflip = ~pgflip & XPBSYR;
	
	VIP_REGS[XPCTRL] = VIP_REGS[XPSTTS] | XPEN;
	while(!(VIP_REGS[XPSTTS] & pgflip));
	VIP_REGS[XPCTRL] = VIP_REGS[XPSTTS] & XPENOFF;//Disable drawing
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
