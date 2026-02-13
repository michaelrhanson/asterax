#include <Sound.h>
#include "SAT.h"
#include "AsteraxResources.h"
#include "MainResources.h"
#include "Sounds.h"

#define kChannels	4
#define kAsync 		TRUE
SndChannelPtr 		soundChannel[kChannels];
Boolean			 	soundsOff = FALSE, volZero = FALSE;

Handle shipExplodingSound, levelOverSound, hitAlienSound, alienAlertSound;
#define rLevelOverSound 	240
#define rHitAlienSound 		217
#define rAlienAlertSound	218

void InitSoundChannels() {
	SndChannelPtr mySndChan;
	OSErr 		  err;
	int			  i;

	if (soundsOff) return;
	
	for (i=0;i<kChannels;i++) {	
		mySndChan = (SndChannelPtr)NewPtrClear(sizeof(SndChannel));		
		if (!mySndChan) {
			ParamText("\pOut of memory!  Please increase the memory in the Finder's 'Get Info' box.",0L,0L,0L);
			Alert(rDefaultAlert,0L);
			ExitToShell();
		}
		mySndChan->qLength = stdQLength;	 //128 sound commands
		err = SndNewChannel(&mySndChan, sampledSynth, initMono, nil);
		
		if (err) {
			soundChannel[i] = NULL;
			for (i++;i<kChannels;i++) soundChannel[i] = NULL;
			break;
		}		
		soundChannel[i] = mySndChan;
	}

	levelOverSound = SATGetSound(rLevelOverSound);
	shipExplodingSound = SATGetSound(rShipExplodesSound);
	hitAlienSound = SATGetSound(rHitAlienSound);
}

void PlaySound(int whichChannel, Handle theSound) {
	OSErr err;
	if (soundsOff || volZero) return;
	if (whichChannel > kChannels || whichChannel <0) return;

	if (!soundChannel[whichChannel]) {
		whichChannel -=2;
		if (whichChannel < 0) return;
	}
	ShutUpChannel(whichChannel);
	err = SndPlay(soundChannel[whichChannel],theSound,kAsync);
}

void ShutUpChannel(int whichChannel) {
	OSErr err;
	SndCommand cmd;
	if (soundsOff || volZero) return;
	if (whichChannel > kChannels || whichChannel <0) return;
	if (!soundChannel[whichChannel]) return;
	
	cmd.cmd = flushCmd;
	cmd.param1=0;
	cmd.param2=0;
	err = SndDoImmediate(soundChannel[whichChannel],&cmd);
	cmd.cmd = quietCmd;
	err = SndDoImmediate(soundChannel[whichChannel],&cmd);
}

void ShutUpAllChannels() {
	int i;
	for (i=0;i<kChannels;i++) ShutUpChannel(i);
}

void PlayLevelOverSound() {
	PlaySound(kYummyChannel, levelOverSound);
}

void PlayShipExplodingSound( ) {
	PlaySound(kExplosionChannel, shipExplodingSound);
}

void PlayAlienHitSound() {
	PlaySound(kExplosionChannel, hitAlienSound);
}

void PlayAlienAlertSound() {
//	PlaySound(kExplosionChannel, alienAlertSound);
}

void DontUseSounds(void) {
	soundsOff = TRUE;
}

void SetVolZero(Boolean t) {
	volZero = t;
}

Boolean UseSounds(void) {
	return !soundsOff;
}