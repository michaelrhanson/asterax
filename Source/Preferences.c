#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AsteraxResources.h"
#include "main.fp"
#include "Preferences.h"
#include <GestaltEqu.h>
#include "Sounds.h"
#include "Playership.h"

/************** Prototypes *****************/
void PlayMoneySound(void);
void GetUnusualRect(Rect *r);
void RGBFore(unsigned int red,unsigned int green,unsigned int blue);

/************** Constants *****************/
#define rScreenDlog				300
#define prefFileName 			"\pAsterax Prefs"
#define keyNum	  			    6
#define kNumScreenSizes 		6
#define rPrefConvertDialog 		303

/************** Types *****************/
typedef struct {
	int					level;

	int					p1lives, p1money;
	long				p1score;
	PlayerShipData 		p1ship, p1insur;
	Boolean				p1hasinsur;
	
	int					p2lives, p2money;
	long				p2score;
	PlayerShipData 		p2ship, p2insur;
	Boolean				p2hasinsur;
} SuspGame;

typedef struct {
	Rect		gameRect;
	char		p1keys[keyNum],p2keys[keyNum];
	short    	volumeSetting;
	int			difficulty;
	short		gameSpeed;
	Boolean		fastDraw;
	Boolean		newRules;
	SuspGame	suspendedGame;
} prefStruct;

typedef struct {
	Rect	gameRect;
	char	p1keys[keyNum],p2keys[keyNum];
	short    volumeSetting;
	int		difficulty;
	Boolean	fastDraw;
} oldPrefStruct;

/************** Globals *****************/
short 					prefsFileRefNum;
prefStruct 				prefs;
static int 				screenWidths[6] = {512,640,640,832,1024,1152};
static int 				screenHeights[6] = {384,400,480,624,768,870};


/************** Code *****************/
void LoadPreferences() {
	// if no preferences file exists, this routine must create one.
	short 		refNum;
	OSErr 		iErr;
	Str255 		buffer;
	long 		size,curEOF;
	char 		*name = "Preferences";
	int 		i;

	memset(&prefs,0,sizeof(prefStruct));

	iErr = FSOpen(prefFileName,0,&refNum);

	curEOF = 0;
	if (!iErr) iErr = GetEOF(refNum,&curEOF);
	if (curEOF == sizeof(oldPrefStruct)) {
		DialogPtr 	d;
		GrafPtr 	oldPort;
		short		iType, item;
		Rect		iRect;
		Handle		iHandle;
		oldPrefStruct oldPrefs;

		GetPort(&oldPort);
		d = GetNewDialog(rPrefConvertDialog, NULL, (WindowPtr)-1L);
		SetPort(d);
		GetDItem(d,1,&iType,&iHandle,&iRect);
		InsetRect(&iRect, -4, -4);PenSize(3,3);FrameRoundRect(&iRect,16,16);PenSize(1,1);
		do {
			ModalDialog(NULL, &item);
		} while (item >=3);
		if (item == 2) ExitToShell();
		size = sizeof(oldPrefStruct);

		iErr = FSRead(refNum,&size,&oldPrefs);
		switch (iErr) {
			case 0:break;
			case eofErr:HandleTerminalError(rFileErrors,rObsoleteFileErr,name);
			case ioErr:HandleTerminalError(rFileErrors,rIoReadErr,name);
			default:HandleTerminalError(rFileErrors,rGenericReadErr,name);
		}
		prefs.gameRect = oldPrefs.gameRect;
		for (i=0;i<keyNum;i++) {
			prefs.p1keys[i] = oldPrefs.p1keys[i];
			prefs.p2keys[i] = oldPrefs.p2keys[i];
		}
		prefs.gameRect = oldPrefs.gameRect;
		prefs.volumeSetting = oldPrefs.volumeSetting;
		prefs.difficulty = oldPrefs.difficulty;
		prefs.fastDraw = oldPrefs.fastDraw;
		prefs.gameSpeed = 2;
		prefs.newRules = TRUE;
		DisposeDialog(d);
		SetPort(oldPort);
	} else {
		size = sizeof(prefStruct);
		if (iErr == fnfErr) {
			SetDefaultPrefs();
			DoScreenPrefsDialog(TRUE);
	
			iErr = Create(prefFileName,0,rMyCreator,rPrefsFileType);
			if (!iErr) {
				iErr = FSOpen(prefFileName,0,&refNum);
				if (iErr) HandleTerminalError(rFileErrors,rGenericOpenErr,name);
				iErr = SetEOF(refNum,size);
			}
			switch (iErr) {
				case 0:break;
				case dskFulErr:case dirFulErr:HandleTerminalError(rFileErrors,rDirFull,name);
				case ioErr:HandleTerminalError(rFileErrors,rIoErr,name);
				case vLckdErr:HandleTerminalError(rFileErrors,rLockedErr,name);
				case wPrErr:HandleTerminalError(rFileErrors,rPermErr,name);
				default:HandleTerminalError(rFileErrors,rGenericCreateErr,name);
			}
			iErr = FSWrite(refNum,&size,&prefs);
			// error return?
		} else if (iErr) {
			HandleTerminalError(rFileErrors,rGenericOpenErr,name);
		} else {
			iErr = FSRead(refNum,&size,&prefs);
			switch (iErr) {
				case 0:break;
				case eofErr:HandleTerminalError(rFileErrors,rObsoleteFileErr,name);
				case ioErr:HandleTerminalError(rFileErrors,rIoReadErr,name);
				default:HandleTerminalError(rFileErrors,rGenericReadErr,name);
			}
		}
	}
	prefsFileRefNum = refNum;
	gGameRect = prefs.gameRect;
	SetSoundVol(prefs.volumeSetting); 
}

void SetDefaultPrefs() {
	prefs.gameRect = screenBits.bounds;
	gGameRect = prefs.gameRect;

	prefs.p1keys[0] = 0x5B;
	prefs.p1keys[1] = 0x58;
	prefs.p1keys[2] = 0x56;
	prefs.p1keys[3] = 0x3B;
	prefs.p1keys[4] = 0x59;
	prefs.p1keys[5] = 0x5c;
	
	prefs.p2keys[0] = 0x10;
	prefs.p2keys[1] = 0x26;
	prefs.p2keys[2] = 0x05;
	prefs.p2keys[3] = 0x37;
	prefs.p2keys[4] = 0x00;
	prefs.p2keys[5] = 0x01;	

	prefs.volumeSetting=4;
	prefs.difficulty = kNormalDif;
	prefs.fastDraw = TRUE;
	prefs.gameSpeed = 2;
	prefs.newRules = TRUE;
	
	prefs.suspendedGame.level = 0;
}

void ChangeKey(char newkey, int player, int num) {
	int i;
	
	if (newkey == escapeKey ||
		newkey == capsLock) {
		PlayMinePing();
		return;
	}

	if (player == 1) {
		prefs.p1keys[num] = newkey;
	} else {
		prefs.p2keys[num] = newkey;
	}
}

char GetPlayerKey(int player, int num) {
	if (player == 1) {
		return prefs.p1keys[num];
	} else {
		return prefs.p2keys[num];
	}
}

void LoadPlayerKeys(int player, PlayerShipData *data) {
	data->leftKey = GetPlayerKey(player,2);
	data->rightKey = GetPlayerKey(player,1);
	data->thrustKey = GetPlayerKey(player,0);
	data->fireKey = GetPlayerKey(player,3);
	data->selSpecialKey = GetPlayerKey(player,4);
	data->actSpecialKey = GetPlayerKey(player,5);
}

void UpdatePrefsFile() {
	long size;
	OSErr myErr;
	size = sizeof(prefStruct);

	myErr = SetFPos(prefsFileRefNum,1,0);
	myErr = FSWrite(prefsFileRefNum,&size,&prefs);
}

void ClosePrefsFile() {
	OSErr iErr;
	iErr = FSClose(prefsFileRefNum);
}

int DetermineCurrentScreenSize() {
	int width, height;
	int i;
	width = gGameRect.right - gGameRect.left;
	height = gGameRect.bottom - gGameRect.top;
	
	for (i=0;i<kNumScreenSizes;i++) {
		if (width < screenWidths[i] || height < screenHeights[i]) break;
	}
	return i-1;
}

void AllowScreenSizeLimits(DialogPtr d, Boolean *support) {
	int 			i;
	Handle 			ctlHand;
	short 			item;
	Rect 			itemRect;
	
	for (i=0;i < kNumScreenSizes; i++) {
		GetDItem(d,5+i,&item,&ctlHand,&itemRect);
		if (screenBits.bounds.right >= screenWidths[i] && 
			screenBits.bounds.bottom >= screenHeights[i]) {

			support[i]=TRUE;
			HiliteControl((ControlHandle)ctlHand,0);
		} else {
			HiliteControl((ControlHandle)ctlHand,255);
		}
	}
}

// returns TRUE if the screen size is changed
Boolean DoScreenPrefsDialog(Boolean quitIfCancel) {
	DialogPtr 		d;
	short 			item;
	int 			oldItem,sel, entrySettings;
	GrafPtr 		oldPort;
	Handle 			ctlHand;
	Rect 			itemRect, newRect, unusualRect;
	Boolean 		support[6], retVal;
	Boolean			unusual = FALSE;
	
	GetPort(&oldPort);
	d = GetNewDialog(rScreenDlog,0L,(WindowPtr)-1L);
	SetPort(d);
	
	entrySettings = sel = DetermineCurrentScreenSize();
	AllowScreenSizeLimits(d, support);

	GetDItem(d,5+sel,&item,&ctlHand,&itemRect);
	SetCtlValue((ControlHandle)ctlHand,TRUE);

	do {
		ModalDialog(0L,&item);
		if (item > 4 && item < 11) {
			oldItem = item;
			GetDItem(d,5+sel,&item,&ctlHand,&itemRect);
			SetCtlValue((ControlHandle)ctlHand,FALSE);
			sel = oldItem - 5;
			GetDItem(d,5+sel,&item,&ctlHand,&itemRect);
			SetCtlValue((ControlHandle)ctlHand,TRUE);		
		}
		if (item == 12) {
			GetUnusualRect(&unusualRect);
			unusual = TRUE;
			GetDItem(d,5+sel,&item,&ctlHand,&itemRect);
			SetCtlValue((ControlHandle)ctlHand,FALSE);
		}
	} while (item > 2);

	if (item == 2) {
		if (quitIfCancel) ExitToShell();
		SetPort(oldPort);
		DisposeDialog(d);
		return FALSE;
	}

	if (unusual) {
		newRect = unusualRect;
	} else {
		SetRect(&newRect, 0,0,screenWidths[sel],screenHeights[sel]);
	}
	
	OffsetRect(&newRect,(screenBits.bounds.right-newRect.right)/2,
								(screenBits.bounds.bottom-newRect.bottom)/2);

	prefs.gameRect = newRect;

	DisposeDialog(d);
	SetPort(oldPort);
	return (sel != entrySettings);
}

void GetUnusualRect(Rect *r) {
	DialogPtr 		d;
	short 			item;
	Rect 			itemRect;
	Handle			hand;
	char			buffer[64];
	long			num, width, height;
	
	d = GetNewDialog(302,0L,(WindowPtr)-1L);
	
	GetDItem(d,5,&item,&hand,&itemRect);
	num = screenBits.bounds.right - screenBits.bounds.left;
	NumToString(num,(StringPtr)buffer);
	SetIText(hand,(StringPtr)buffer);
	
	GetDItem(d,6,&item,&hand,&itemRect);
	num = screenBits.bounds.bottom - screenBits.bounds.top;
	NumToString(num,(StringPtr)buffer);
	SetIText(hand,(StringPtr)buffer);

	while (TRUE) {
		while (TRUE) {
			ModalDialog(0L,&item);
			if (item==1) break;
		}
		GetDItem(d,6,&item,&hand,&itemRect);
		GetIText(hand,(StringPtr)buffer);
		StringToNum((StringPtr)buffer,&height);
		
		GetDItem(d,5,&item,&hand,&itemRect);
		GetIText(hand,(StringPtr)buffer);
		StringToNum((StringPtr)buffer,&width);

		if (height > screenBits.bounds.bottom - screenBits.bounds.top ||
			width > screenBits.bounds.right - screenBits.bounds.left ||
			height < 384 || width < 512) {
				SysBeep(0);
		} else {
			break;	
		}
	}
	r->left = 0;
	r->top = 0;
	r->right = width;
	r->bottom = height;
	DisposeDialog(d);
}

Boolean PrefFastDraw(void) {
	if (!prefs.fastDraw) RGBFore(0,0,0);
	return prefs.fastDraw;
}

void TogglePrefFastDraw() {
	prefs.fastDraw = !prefs.fastDraw;
}

int PrefDifficulty(void) {
	return prefs.difficulty;
}

int GetPrefSpeed(void) {
	return prefs.gameSpeed;
}

void TogglePrefSpeed(void) {
	prefs.gameSpeed = 3-prefs.gameSpeed;
}

void SetPrefDifficulty(int dif) {
	prefs.difficulty = dif;
}

void ChangePrefVolumeLevel(int delta) {
	prefs.volumeSetting+=delta;
	if (prefs.volumeSetting<0) prefs.volumeSetting =0;
	if (prefs.volumeSetting>7) prefs.volumeSetting =7;

	if (prefs.volumeSetting == 0) {
		SetVolZero(TRUE);	
	} else {
		SetVolZero(FALSE);	
	}

	SetSoundVol(prefs.volumeSetting);
	PlayMoneySound();
}

Boolean GetPrefNewRules(void) {
	return prefs.newRules;
}

void TogglePrefNewRules(void) {
	prefs.newRules = !prefs.newRules;
}

void SetSuspendParameters(int level, long p1score, int p1lives, int p1money, PlayerShipData *p1ship, PlayerShipData *p1insur,
						  long p2score, int p2lives, int p2money, PlayerShipData *p2ship, PlayerShipData *p2insur) {
						  
	prefs.suspendedGame.level = level;
	prefs.suspendedGame.p1lives = p1lives;
	prefs.suspendedGame.p1score = p1score;
	prefs.suspendedGame.p1money = p1money;
	if (p1lives) prefs.suspendedGame.p1ship = *p1ship;
	if (p1insur) {
		prefs.suspendedGame.p1insur = *p1insur;
		prefs.suspendedGame.p1hasinsur = TRUE;
	} else {
		prefs.suspendedGame.p1hasinsur = FALSE;
	}
						  
	if (p2score) {
		prefs.suspendedGame.p2lives = p2lives;
		prefs.suspendedGame.p2score = p2score;
		prefs.suspendedGame.p2money = p2money;
		if (p2lives) prefs.suspendedGame.p2ship = *p2ship;
		if (p2insur) {
			prefs.suspendedGame.p2insur = *p2insur;
			prefs.suspendedGame.p2hasinsur = TRUE;
		} else {
			prefs.suspendedGame.p2hasinsur = FALSE;
		}
	}
}

void GetSuspendParameters(
					int *level, long *p1score, int *p1lives, int *p1money, 
					PlayerShipData **p1ship, PlayerShipData **p1insur,
					long *p2score, int *p2lives, int *p2money, 
					PlayerShipData **p2ship, PlayerShipData **p2insur) {
						  
	*level   = prefs.suspendedGame.level;
	*p1score = prefs.suspendedGame.p1score;
	*p1money = prefs.suspendedGame.p1money;
	*p1lives = prefs.suspendedGame.p1lives;

	if (*p1lives) {
		*p1ship = 	(PlayerShipData *)NewPtr(sizeof(PlayerShipData));
		**p1ship = prefs.suspendedGame.p1ship;
	}
	if (prefs.suspendedGame.p1hasinsur) {
		*p1insur = 	(PlayerShipData *)NewPtr(sizeof(PlayerShipData));
		**p1insur = prefs.suspendedGame.p1insur;
	} else {
		*p1insur = NULL;
	}
						  
	if (prefs.suspendedGame.p2score) {
		*p2lives = prefs.suspendedGame.p2lives;
		*p2score = prefs.suspendedGame.p2score;
		*p2money = prefs.suspendedGame.p2money;
		if (*p2lives) {
			*p2ship = 	(PlayerShipData *)NewPtr(sizeof(PlayerShipData));
			**p2ship = prefs.suspendedGame.p2ship;
		}
		if (prefs.suspendedGame.p2hasinsur) {
			*p2insur = 	(PlayerShipData *)NewPtr(sizeof(PlayerShipData));
			**p2insur = prefs.suspendedGame.p2insur;
		} else {
			*p2insur = NULL;
		}
	} else {
		*p2score = 0;
		*p2lives = 0;
		*p2money = 0;
	}
	prefs.suspendedGame.level = 0; 
	prefs.suspendedGame.p2score = 0;
	prefs.suspendedGame.p2lives = 0;
	prefs.suspendedGame.p2money = 0;
}

Boolean SuspendedGameAvailable() {
	return ((prefs.suspendedGame.level)!=0);
}

void GetSuspendedSummary(int *level, int *nPlayers) {
	*level = prefs.suspendedGame.level;
	*nPlayers = ((prefs.suspendedGame.p2score)!=0)?2:1;
}