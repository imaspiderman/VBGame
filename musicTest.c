#include <libgccvb.h>
#include "starwars.h"

u32 soundStart = 0;

void DoSound(){
	if(soundStart==0 || soundStart>(STARWARS_LEN>>1)){
		u8 i;
		soundStart=0;
		
		timer_freq(1);
		timer_set(TIME_MS(1));
		timer_enable(1);
		timer_int(1);
		
		//Sound code for no interrupt
		/*
		while(soundStart<STARWARS_LEN){//DogP's wavonvb.c example code
			if (!(soundStart&1)) //first half of packed data
				SND_REGS[0].SxLRV=(starwars[soundStart>>1]&0xF0)|(starwars[soundStart>>1]>>4);
			else //second half of packed data
				SND_REGS[0].SxLRV=((starwars[soundStart>>1]&0x0F)<<4)|(starwars[soundStart>>1]&0x0F);
			for(i=0;i<32;i++);//slight delay
			
			soundStart++;

			SND_REGS[0].SxINT = 0xBF;//Start sound
			while(SND_REGS[0].SxINT & 80);//Wait for interval to reach 0
		} 
		*/
	}
}

void timeHnd(void){
	u8 loop,i;
	
	timer_int(0);
	timer_enable(0);
	timer_clearstat();
	
	loop=0;
	while(soundStart<STARWARS_LEN && loop < 8){//DogP's wavonvb.c example code
		if (!(soundStart&1)) //first half of packed data
			SND_REGS[0].SxLRV=(starwars[soundStart>>1]&0xF0)|(starwars[soundStart>>1]>>4);
		else //second half of packed data
			SND_REGS[0].SxLRV=((starwars[soundStart>>1]&0x0F)<<4)|(starwars[soundStart>>1]&0x0F);
		//for(i=0;i<18;i++);//slight delay
		
		soundStart++;
		loop++;

		SND_REGS[0].SxINT = 0x9F;//Start sound (That's how I usually start it)
		//while(SND_REGS[0].SxINT & 80);//Wait for interval to reach 0 (Doesn't seem to work on hardware)
	} //Do 4 bytes of data per interrupt
	if(soundStart<STARWARS_LEN){
		timer_int(1);
		timer_enable(1);
	} else soundStart = 0;
}

void vbInit(){
	vbDisplayOn ();
	vbSetColTable ();
	
	VIP_REGS[BRTA]  = 32;
	VIP_REGS[BRTB]  = 64;
	VIP_REGS[BRTC]  = 32;
	
	HW_REGS[WCR] = 1;
	
	tim_vector = (u32)timeHnd;
}

const u8 kModData[] = {
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 16, 17, 18, 19, 20, 21, -1, -2, -3, -4, -5,
  -6, -7, -8, -9, -16, -17, -18, -19, -20, -21, -22
};

int main(){
	u16 i;
	
	//For me it sounds like the modulation data reduces the background noise
	for(i = 0; i <= 0x7C; i++) 
	{	
		MODDATA[i << 2] = kModData[i];
	}
	
	for(i=0;i<32;i++)WAVEDATA1[i<<2] = 0x3F;//DC wave
	
	vbInit();
	SSTOP = 1;
	SND_REGS[0].SxEV0 = 0xFC;         	// No fadeout; volume is constant.
    SND_REGS[0].SxEV1 = 0x02;         	// Repeat it forever.
	SND_REGS[0].SxRAM = 0x00;
	SND_REGS[0].SxFQH = 0x00;
	SND_REGS[0].SxFQL = 0x00;
	
	SSTOP = 0;
	
	
	
	
	DoSound(); //only start the interupt handler once
	
	while(1){
			//while(!(vbReadPad() & K_ANY));
			
	}
}