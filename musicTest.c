#include <libgccvb.h>
#include "Music.h"

#define MAX_NOTE_LENGTH 150

u32 soundStart = 0;

midiTrack t1 = {0,(midiNote*)track_0,1,0,0x77};
#define NUM_TRACKS 1
midiTrack trackTable[NUM_TRACKS];

void DoSound(u16 timerCount){
	u8 i;
	for(i=0; i<NUM_TRACKS; i++){
		trackTable[i].currNote = 0;
	}
	musicTick = 0;
	timer_freq(1);
	timer_set(timerCount);
	timer_enable(1);
	timer_int(1);
	
	HW_REGS[BKCOL] = ~HW_REGS[BKCOL];
}

void timeHnd(void){
	u8 tr,doTimer;
	timer_int(0);
	timer_enable(0);
	timer_clearstat();
	
	for(tr=0; tr<NUM_TRACKS; tr++){
		if(trackTable[tr].play == 0) continue;
		if(trackTable[tr].data[trackTable[tr].currNote].tickPosition == musicTick && 
		   trackTable[tr].data[trackTable[tr].currNote].onoff == 0x09){//Start note
		    while(trackTable[tr].data[trackTable[tr].currNote+1].tickPosition == musicTick && //Look for the last note in a cord and play that one.
			      trackTable[tr].data[trackTable[tr].currNote+1].onoff == 0x09) trackTable[tr].currNote++;
			SND_REGS[tr].SxFQH=(FREQUENCY[trackTable[tr].data[trackTable[tr].currNote].note]>>8) & 0xFF;
			SND_REGS[tr].SxFQL=FREQUENCY[trackTable[tr].data[trackTable[tr].currNote].note] & 0xFF;
			SND_REGS[tr].SxLRV=trackTable[tr].volume;
			SND_REGS[tr].SxRAM = tr;
			trackTable[tr].offNote = 0;
			SND_REGS[tr].SxINT = 0x9F;
		}
		/*some midi's don't have off notes so this sets a max length for a given note*/
		trackTable[tr].offNote++;
		if(trackTable[tr].offNote > MAX_NOTE_LENGTH){
			SND_REGS[tr].SxINT = 0x00;
		}

		while(trackTable[tr].data[trackTable[tr].currNote].tickPosition <= musicTick && 
		      trackTable[tr].data[trackTable[tr].currNote].tickPosition != 0xFFFFFFFF){
			trackTable[tr].currNote++;
		}
		
		if(trackTable[tr].data[trackTable[tr].currNote].tickPosition == 0xFFFFFFFF){
			trackTable[tr].play = 0;
			trackTable[tr].currNote = 0;
			SND_REGS[tr].SxINT=0;
		}
	}
	musicTick++;
	doTimer = 0x00;
	for(tr=0; tr<NUM_TRACKS; tr++){
		if(trackTable[tr].play == 1) doTimer = 0x01;
	}
	if(doTimer){
		timer_int(1);
		timer_enable(1);
	}
}

void vbInit(){
	vbDisplayOn ();
	vbSetColTable ();
	
	VIP_REGS[BRTA]  = 32;
	VIP_REGS[BRTB]  = 64;
	VIP_REGS[BRTC]  = 32;
	
	HW_REGS[WCR] = 1;
	
	tim_vector = (u32)timeHnd;
	
	trackTable[1] = t1;
}

const u8 kModData[] = {
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 16, 17, 18, 19, 20, 21, -1, -2, -3, -4, -5,
  -6, -7, -8, -9, -16, -17, -18, -19, -20, -21, -22
};

int main(){
	u16 i;
	
	SSTOP = 0;
	
	//For me it sounds like the modulation data reduces the background noise
	for(i = 0; i <= 0x7C; i++) 
	{	
		MODDATA[i << 2] = kModData[i];
	}
	
	for(i=0;i<32;i++)WAVEDATA1[i<<2] = TRUMPET[i];//Instrument
	for(i=0;i<32;i++)WAVEDATA2[i<<2] = PIANO[i];//Instrument
	for(i=0;i<32;i++)WAVEDATA3[i<<2] = VIOLIN[i];//Instrument
	
	
	vbInit();
	SSTOP = 1;
	SND_REGS[0].SxEV0 = 0xFC;         	// No fadeout; volume is constant.
    SND_REGS[0].SxEV1 = 0x02;         	// Repeat it forever.
	SND_REGS[0].SxRAM = 0x00;
	SND_REGS[0].SxFQH = 0x00;
	SND_REGS[0].SxFQL = 0x00;
	SND_REGS[0].SxLRV = 0x33;
	
	SND_REGS[1].SxEV0 = 0xFC;         	// No fadeout; volume is constant.
    SND_REGS[1].SxEV1 = 0x02;         	// Repeat it forever.
	SND_REGS[1].SxRAM = 0x00;
	SND_REGS[1].SxFQH = 0x00;
	SND_REGS[1].SxFQL = 0x00;
	SND_REGS[1].SxLRV = 0x33;
	
	SND_REGS[2].SxEV0 = 0xFC;         	// No fadeout; volume is constant.
    SND_REGS[2].SxEV1 = 0x02;         	// Repeat it forever.
	SND_REGS[2].SxRAM = 0x00;
	SND_REGS[2].SxFQH = 0x00;
	SND_REGS[2].SxFQL = 0x00;
	SND_REGS[2].SxLRV = 0x33;
	
	while(1){
			while(!(vbReadPad() & K_ANY));
			DoSound(52);
	}
}