#include "gamma.h"

Boolean UseGamma = FALSE;
void TurnOffGamma(void);


void FadeDown(int speed) {
	int i;
	if (!UseGamma) return;
	if (IsGammaAvailable()) {
		for (i=100;i>=0;i-=speed) {
			DoGammaFade(i);			
		}
		DoGammaFade(0);
	}
}

void FadeUp(int speed) {
	int i;
	if (!UseGamma) return;
	if (IsGammaAvailable()) {
		for (i=0;i<100;i+=speed) DoGammaFade(i);
		DoGammaFade(100);
	}
}


void CheckForFader(void) {
	OSErr 	err;
	Handle	hRsrc;
	unsigned char	*data;
	
	return; // do nothing: fades are too damn dangerous
	hRsrc = GetResource('PREF',128);
	HLock(hRsrc);
	data = *(unsigned char **)hRsrc;
	
	if (*data & 0x80) {
		if (IsGammaAvailable()) {
			err = SetupGammaTools();
			if (err) TurnOffGamma();
		}
	} else {
		TurnOffGamma();
	}
	HUnlock(hRsrc);
}

void TurnOffGamma(void) {
	UseGamma = FALSE;
}

void DeactivateFade(void) {
	if (UseGamma) DisposeGammaTools();
}