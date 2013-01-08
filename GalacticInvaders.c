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
		drawObject(starFoxShip,4,4,3);
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
		starFoxShip->rz-=ROTATION_SPEED;
		if(starFoxShip->rz<=-360 || starFoxShip->rz>=360) starFoxShip->rz=0;
	}
	if(K_LR & buttons){
		starFoxShip->rz+=ROTATION_SPEED;
		if(starFoxShip->rz>=360 || starFoxShip->rz<=-360) starFoxShip->rz=0;
	}
	if(K_LU & buttons){
		starFoxShip->rx-=ROTATION_SPEED;
		if(starFoxShip->rx<=-360 || starFoxShip->rx>=360) starFoxShip->rx=0;
	}
	if(K_LD & buttons){
		starFoxShip->rx+=ROTATION_SPEED;
		if(starFoxShip->rx>=360 || starFoxShip->rx<=-360) starFoxShip->rx=0;
	}
	if(K_RL & buttons){
		starFoxShip->ry-=ROTATION_SPEED;
		if(starFoxShip->ry<=-360 || starFoxShip->ry>=360) starFoxShip->ry=0;
	}
	if(K_RR & buttons){
		starFoxShip->ry+=ROTATION_SPEED;
		if(starFoxShip->ry>=360 || starFoxShip->ry<=-360) starFoxShip->ry=0;
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
	o->wx+=(d&0x01)?(o->speed.x):(0);	
	o->wy+=(d&0x02)?(o->speed.y):(0);
	o->wz+=(d&0x04)?(o->speed.z):(0);
	
	//Calculate clip bounds if object moved
	//This is not a true clip function but should
	//satisify simple needs providing objects aren't too
	//large.
	
	//EYE_Z is 128 or 2^7 so we can shift it.
	//We loose some accuracy but it's worth it to not have
	//to divide.
	
	//(-eyex*z)/eyez = xmin
	o->c_min_x = NUM_SCALE_DN((-EYE_X*o->wz));
	//(-eyey*z)/eyez = ymin
	o->c_min_y = NUM_SCALE_DN((-EYE_Y*o->wz));
	//((screen_max-eyex)/eyez) * z + eyex = xmax
	o->c_max_x = NUM_SCALE_DN((SCREEN_WIDTH-EYE_X)) * o->wz + EYE_X;
	//((screen_max-eyey)/eyez) * z + eyey = ymax
	o->c_max_y = ((NUM_SCALE_DN((SCREEN_HEIGHT-EYE_Y))==0)?(1):(SCREEN_HEIGHT-EYE_Y))* o->wz + EYE_Y;
	o->c_min_z = EYE_Z;
	o->c_max_z = 10000;
	
	/*
	if(d&0x08){
		s32 x = ((o->wx-EYE_X)*EYE_Z)/(EYE_Z+o->wz)+EYE_X;
		s32 y = ((o->wy-EYE_Y)*EYE_Z)/(EYE_Z+o->wz)+EYE_Y;
		
		if(x<0 && d&0x01)  o->wx=sLeft;
		if(x>SCREEN_WIDTH && d&0x01) o->wx=sRight;
		if(y<0 && d&0x02) o->wy=sTop;
		if(y>SCREEN_HEIGHT && d&0x02) o->wy=sBottom;
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
	vector3d vt,vt2;
	matrix3d m_temp;
	o->p = ((o->wz-EYE_Z)>>PARALLAX_SHIFT);
	
	translateMatrix(m_translate3d, o->wx, o->wy, o->wz);
	rotateMatrixX(m_rotate3d_x,o->rx);
	rotateMatrixY(m_rotate3d_y,o->ry);
	rotateMatrixZ(m_rotate3d_z,o->rz);
	scaleMatrix(m_scale3d,xscale,yscale,zscale);
	
	matrix3dxMatrix3d(m_scale3d,m_translate3d,m_temp);
	copyMatrix(m_temp,m_world3d);
	matrix3dxMatrix3d(m_world3d,m_rotate3d_x,m_temp);
	copyMatrix(m_temp,m_world3d);
	matrix3dxMatrix3d(m_world3d,m_rotate3d_y,m_temp);
	copyMatrix(m_temp,m_world3d);
	matrix3dxMatrix3d(m_world3d,m_rotate3d_z,m_temp);
	copyMatrix(m_temp,m_world3d);

	_CacheEnable
	i=0;
	while(i<o->dataSize){
		//Translate vertices if needed
		v1.x = o->data[i];
		v1.y = o->data[i+1];
		v1.z = o->data[i+2];
		
		matrix3dxVertex(&v1,m_world3d,&vt);
		
		v1.x = vt.x;
		v1.y = vt.y;
		v1.z = vt.z;
		
		v2.x = o->data[i+3];
		v2.y = o->data[i+4];
		v2.z = o->data[i+5];
		
		matrix3dxVertex(&v2,m_world3d,&vt);
		
		v2.x = vt.x;
		v2.y = vt.y;
		v2.z = vt.z;
		
		v3.x = o->data[i+6];
		v3.y = o->data[i+7];
		v3.z = o->data[i+8];
		
		matrix3dxVertex(&v3,m_world3d,&vt);
		
		v3.x = vt.x;
		v3.y = vt.y;
		v3.z = vt.z;

		//crossProduct(va,vb);//fills (vector3d)xProduct
		//dotp=dotProduct(cameraToPoly,xProduct);
		//if visible dotp will be > 0
		/*
		if(dotp<0) {
			i+=9;
			continue;
		}
		*/
		drawLine(v1,v2,3,o);
		drawLine(v2,v3,3,o);
		drawLine(v3,v1,3,o);
		i+=9;
		
	}
	_CacheDisable
}

void initObjects(){
	gameObjectsIdx=0;
	gameObjects[gameObjectsIdx].wx=EYE_X;
	gameObjects[gameObjectsIdx].wy=EYE_Y;
	gameObjects[gameObjectsIdx].wz=EYE_Z+100;
	gameObjects[gameObjectsIdx].rx=0;
	gameObjects[gameObjectsIdx].ry=0;
	gameObjects[gameObjectsIdx].rz=0;
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
	o->x = NUM_SCALE_DN((v->x*m[0][0])) +
	       NUM_SCALE_DN((v->y*m[1][0])) +
	       NUM_SCALE_DN((v->z*m[2][0]));
	      
	o->y = NUM_SCALE_DN((v->x*m[0][1])) +
	       NUM_SCALE_DN((v->y*m[1][1])) +
	       NUM_SCALE_DN((v->z*m[2][1]));
	      
	o->z = NUM_SCALE_DN((v->x*m[0][2])) +
	       NUM_SCALE_DN((v->y*m[1][2])) +
	       NUM_SCALE_DN((v->z*m[2][2]));
}

/************************************
Multiply matrices together to produce
single transform matrix.
************************************/
void matrix3dxMatrix3d(matrix3d m1, matrix3d m2, matrix3d n){
	u8 i,j,k,c,r;
	s32 t;
	i=j=k=r=c=0;
	//Multiply m1 cols by m2 rows
	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			for(k=0;k<4;k++){
				t+=NUM_SCALE_DN((m2[k][j])*(m1[i][k]));
			}
			n[r][c]=t;
			c++;
			t=0;
		}
		r++;
		c=0;
	}
	
}

/*******************************
Fills the rotate matrix for z axis
angle is in degrees.
*******************************/
void rotateMatrixZ(matrix3d m,s32 angle){
	angle = (angle<0)?(360+angle):(angle);
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
	m[2][2]=NUM_SCALE_UP(1);
	m[2][3]=0;
	
	m[3][0]=0;
	m[3][1]=0;
	m[3][2]=0;
	m[3][3]=NUM_SCALE_UP(1);
}

/*******************************
Fills the rotate matrix for y axis
angle is in degrees.
*******************************/
void rotateMatrixY(matrix3d m,s32 angle){
	angle = (angle<0)?(360+angle):(angle);
	m[0][0]=cosine[angle];
	m[0][1]=0;
	m[0][2]=-(sine[angle]);
	m[0][3]=0;
	
	m[1][0]=0;
	m[1][1]=NUM_SCALE_UP(1);
	m[1][2]=0;
	m[1][3]=0;
	
	m[2][0]=sine[angle];
	m[2][1]=0;
	m[2][2]=cosine[angle];
	m[2][3]=0;
	
	m[3][0]=0;
	m[3][1]=0;
	m[3][2]=0;
	m[3][3]=NUM_SCALE_UP(1);
}

/*******************************
Fills the rotate matrix for x axis
angle is in degrees.
*******************************/
void rotateMatrixX(matrix3d m,s32 angle){
	angle = (angle<0)?(360+angle):(angle);
	m[0][0]=NUM_SCALE_UP(1);
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
	m[3][3]=NUM_SCALE_UP(1);
}

/*********************************
Fills the translation matrix
*********************************/
void translateMatrix(matrix3d m, s32 x, s32 y, s32 z){
	m[0][0]=NUM_SCALE_UP(1);
	m[0][1]=0;
	m[0][2]=0;
	m[0][3]=(x);
	
	m[1][0]=0;
	m[1][1]=NUM_SCALE_UP(1);
	m[1][1]=0;
	m[1][1]=(y);
	
	m[2][0]=0;
	m[2][1]=0;
	m[2][2]=NUM_SCALE_UP(1);
	m[2][3]=(z);
	
	m[3][0]=0;
	m[3][1]=0;
	m[3][2]=0;
	m[3][3]=NUM_SCALE_UP(1);
	
	
}

/*********************************
Fills the camera translation matrix.
Same as regular translate except all
axis scaled by -1.
*********************************/
void translateCameraMatrix(matrix3d m, s32 x, s32 y, s32 z){
	m[0][0]=NUM_SCALE_UP(1);
	m[0][1]=0;
	m[0][2]=0;
	m[0][3]=(-x);
	
	m[1][0]=0;
	m[1][1]=NUM_SCALE_UP(1);
	m[1][1]=0;
	m[1][1]=(-y);
	
	m[2][0]=0;
	m[2][1]=0;
	m[2][2]=NUM_SCALE_UP(1);
	m[2][3]=(-z);
	
	m[3][0]=0;
	m[3][1]=0;
	m[3][2]=0;
	m[3][3]=NUM_SCALE_UP(1);
	
	
}

/********************************
Fills the scale matrix
********************************/
void scaleMatrix(matrix3d m,s32 sx, s32 sy, s32 sz){
	m[0][0]=NUM_SCALE_UP(sx);
	m[0][1]=0;
	m[0][2]=0;
	
	m[1][0]=0;
	m[1][1]=NUM_SCALE_UP(sy);
	m[1][2]=0;
	
	m[2][0]=0;
	m[2][1]=0;
	m[2][2]=NUM_SCALE_UP(sz);
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
void drawLine(vector3d v1, vector3d v2, u8 color, object* o){
	s32 vx,vy,vz,vx2,vy2,vz2;

	//vx=(v1.x<o->c_min_x)?(o->c_min_x):(v1.x);
	//vx=(vx>o->c_max_x)?(o->c_max_x):(vx);
	//vx2=(v2.x<o->c_min_x)?(o->c_min_x):(v2.x);
	//vx2=(vx2>o->c_max_x)?(o->c_max_x):(vx2);
	asm(
		"mov %2,r5\n"//v1.x
		"mov %3,r6\n"//o->c_min_x
		"mov %4,r7\n"//o->c-max_x
		"mov %5,r9\n"//v2.x
		"mov r6,r8\n"
		"cmp r5,r6\n"//o->c_min_x > v1.x
		"bge _drawline5\n"
		"mov r5,r8\n"
		"_drawline5:\n"
		"cmp r8,r7\n"//o->c_max_x > v1.x
		"bge _drawline6\n"
		"mov r7,r8\n"
		"_drawline6:\n"
		"mov r8,%0\n"
		"mov r6,r8\n"
		"cmp r9,r8\n"//o->c_min_x > v2.x
		"bge _drawline7\n"
		"mov r9,r8\n"
		"_drawline7:\n"
		"cmp r8,r7\n"//o->c_max_x > v2.x
		"bge _drawline8\n"
		"mov r7,r8\n"
		"_drawline8:\n"
		"mov r8,%1\n"
		:"=r"(vx),"=r"(vx2)
		:"r"(v1.x),"r"(o->c_min_x),"r"(o->c_max_x),"r"(v2.x)
		:"r5","r6","r7","r8"
		);
	
	//vy=(v1.y<o->c_min_y)?(o->c_min_y):(v1.y);
	//vy=(vy>o->c_max_y)?(o->c_max_y):(vy);
	//vy2=(v2.y<o->c_min_y)?(o->c_min_y):(v2.y);
	//vy2=(vy2>o->c_max_y)?(o->c_max_y):(vy2);
	
	asm(
		"mov %2,r5\n"//v1.y
		"mov %3,r6\n"//o->c_min_y
		"mov %4,r7\n"//o->c-max_y
		"mov %5,r9\n"//v2.y
		"mov r6,r8\n"
		"cmp r5,r6\n"//o->c_min_y > v1.y
		"bge _drawline9\n"
		"mov r5,r8\n"
		"_drawline9:\n"
		"cmp r8,r7\n"//o->c_max_y > v1.y
		"bge _drawline10\n"
		"mov r7,r8\n"
		"_drawline10:\n"
		"mov r8,%0\n"
		"mov r6,r8\n"
		"cmp r9,r8\n"//o->c_min_y > v2.y
		"bge _drawline11\n"
		"mov r9,r8\n"
		"_drawline11:\n"
		"cmp r8,r7\n"//o->c_max_y > v2.y
		"bge _drawline12\n"
		"mov r7,r8\n"
		"_drawline12:\n"
		"mov r8,%1\n"
		:"=r"(vy),"=r"(vy2)
		:"r"(v1.y),"r"(o->c_min_y),"r"(o->c_max_y),"r"(v2.y)
		:"r5","r6","r7","r8"
		);
	
	//vz=(v1.z<o->c_min_z)?(o->c_min_z):(v1.z);
	//vz=(vz>o->c_max_z)?(o->c_max_z):(vz);
	//vz2=(v2.z<o->c_min_z)?(o->c_min_z):(v2.z);
	//vz2=(vz2>o->c_max_z)?(o->c_max_z):(vz2);
	
	asm(
		"mov %2,r5\n"//v1.z
		"mov %3,r6\n"//o->c_min_z
		"mov %4,r7\n"//o->c-max_z
		"mov %5,r9\n"//v2.z
		"mov r6,r8\n"
		"cmp r5,r6\n"//o->c_min_z > v1.z
		"bge _drawline13\n"
		"mov r5,r8\n"
		"_drawline13:\n"
		"cmp r8,r7\n"//o->c_max_z > v1.z
		"bge _drawline14\n"
		"mov r7,r8\n"
		"_drawline14:\n"
		"mov r8,%0\n"
		"mov r6,r8\n"
		"cmp r9,r8\n"//o->c_min_z > v2.z
		"bge _drawline15\n"
		"mov r9,r8\n"
		"_drawline15:\n"
		"cmp r8,r7\n"//o->c_max_z > v2.z
		"bge _drawline16\n"
		"mov r7,r8\n"
		"_drawline16:\n"
		"mov r8,%1\n"
		:"=r"(vz),"=r"(vz2)
		:"r"(v1.z),"r"(o->c_min_z),"r"(o->c_max_z),"r"(v2.z)
		:"r5","r6","r7","r8"
		);
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
		
		:"=r"(vx),"=r"(vx2)
		:"r"(EYE_X),"r"(EYE_Z),"r"(vx),
		 "r"(vz),"r"(vx2),"r"(vz2)
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
		
		:"=r"(vy),"=r"(vy2)
		:"r"(EYE_Y),"r"(EYE_Z),"r"(vy),
		 "r"(vz),"r"(vy2),"r"(vz2)
	);
	/**************************
	The following algorithm was taken from stack overflow
	http://stackoverflow.com/questions/5186939/algorithm-for-drawing-a-4-connected-line
	**************************/
	//dx=abs(vx2-vx);
	//dy=abs(vy2-vy);
	asm(
		"mov %2,r5\n" //x1
		"mov %3,r6\n" //x2
		"mov %4,r7\n" //y1
		"mov %5,r8\n" //y2
		"sub r6,r5\n"
		"bge _drawline1\n"
		"not r5,r5\n"
		"add 1,r5\n"
		"_drawline1:\n"
		"mov r5,%0\n"
		"sub r8,r7\n"
		"bge _drawline2\n"
		"not r7,r7\n"
		"add 1,r7\n"
		"_drawline2:\n"
		"mov r7,%1\n"
		:"=r"(dx),"=r"(dy)
		:"r"(vx),"r"(vx2),"r"(vy),"r"(vy2)
		:"r5","r6","r7","r8"
		);
	
	//sx = (vx<vx2)?(1):(-1);
	//sy = (vy<vy2)?(1):(-1);
	
	asm(
		"mov %2,r5\n" //x1
		"mov %3,r6\n" //x2
		"mov %4,r7\n" //y1
		"mov %5,r8\n" //y2
		"mov 1 ,r9\n"
		"cmp r5,r6\n"
		"bge _drawline3\n"
		"mov -1,r9\n"
		"_drawline3:\n"
		"mov r9,%0\n"
		"mov 1,r9\n"
		"cmp r7,r8\n"
		"bge _drawline4\n"
		"mov -1,r9\n"
		"_drawline4:\n"
		"mov r9,%1\n"
		:"=r"(sx),"=r"(sy)
		:"r"(vx),"r"(vx2),"r"(vy),"r"(vy2)
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
		drawPoint(vx,vy,color,o->p);
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
			//vx+=sx;
			//e=e1;
			asm(
				"mov %2,r5\n" //x1
				"mov %3,r6\n" //sx
				"mov %4,%1\n" //e1
				"add r6,r5\n"
				"mov r5,%0\n"
				:"=r"(vx),"=r"(e)
				:"r"(vx),"r"(sx),"r"(e1)
				:"r5","r6"
				);
		}else{
			//vy+=sy;
			//e=e2;
			asm(
				"mov %2,r5\n" //y1
				"mov %3,r6\n" //sy
				"mov %4,%1\n" //e2
				"add r6,r5\n"
				"mov r5,%0\n"
				:"=r"(vy),"=r"(e)
				:"r"(vy),"r"(sy),"r"(e2)
				:"r5","r6"
				);
		}
	}
}

/*******************************
Dot Product function for determining
the angle between two vectors.
*******************************/
s32 dotProduct(vector3d* v1, vector3d* v2){
	s32 r;
	r = (v1->x*v2->x)+(v1->y*v2->y)+(v1->z*v2->z);
	return r;
}

/*******************************
Cross Product function
Sets the values of xProduct[x,y,z]
*******************************/
void crossProduct(vector3d* v1,vector3d* v2){
	xProduct.x = (v1->y*v2->z)-(v1->z*v2->y);
	xProduct.y = (v1->z*v2->x)-(v1->x*v2->z);
	xProduct.z = (v1->x*v2->y)-(v1->y*v2->x);
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
	length=isqrt((v->x*v->x)+(v->y*v->y)+(v->z*v->z));
	n->x = v->x/length;
	n->y = v->y/length;
	n->z = v->z/length;
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
