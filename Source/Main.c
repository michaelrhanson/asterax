/* Asterax: Two player asteroids */
#include <Traps.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include "SAT.h"
#include "AsteraxResources.h"
#include "Sprites.h"
#include "HighScores.h"
#include <OSUtils.h>
#include <GestaltEqu.h>
#include "MainResources.h"
#include "Game.fp"
#include "Preferences.h"
#include <Palettes.h>
#include "About.h"
#include <Sound.h>
#include "strings.h"

Boolean MyTrapAvailable(short theTrap);
Boolean TrackMouseInButton(PicHandle outPic, PicHandle inPic, Rect r);


void DrawCheat(void);
Boolean HandleCheatModeClick(Point where);


#define TrapMask 0x0800
#define _Gestalt 0xA1AD

Boolean gDone = FALSE;
Boolean fullHighScores;
Boolean gHasGestalt;
Boolean gActive = TRUE;

#define mainScreenMode  0
#define aboutMode		1
#define getKeysMode		2
#define prefsMode		3
#define cheatMode		4

#define kLastAbout		11


struct {
	int screenMode;
	RgnHandle moreHighscoreInfoRgn,	moreHighscoreInfoDownRgn;
	Rect highScoreInfoRect;
	
	int  setKeySelected;

	PicHandle beginOnePict, beginTwoPict, prefPict, keyPict, aboutPict, quitPict;
	PicHandle redIn, redOut, blueIn,blueOut,greenIn,greenOut;
	PicHandle doneIn, doneOut, nextIn, nextOut;
	PicHandle arrowUp, arrowDown, arrowUpLit, arrowDownLit;

	int	 aboutScreenNum;
	Rect beginOneRect, beginTwoRect, prefRect, keyRect, aboutRect, quitRect;
	Rect nextRect, doneRect, highScoreRect;
	Rect p1keyRect, p2keyRect;
	Rect volumeUpRect, volumeDownRect;
	Rect diffUpRect, diffDownRect;

	Rect 	prefScreenRect, prefDrawRect, prefDiffBeginnerRect, prefDiffNormalRect;
	Rect	prefDiffExpertRect, prefSpeedRect, prefRulesRect;
	Boolean		isFaded;
	
	
	short originalVolume;
	int	  originalColorDepth;
	Boolean brightenColors;
} mainWidgets;

ToolBoxInit() {	InitGraf(&thePort);
	InitFonts();
	FlushEvents(everyEvent,0);
	InitWindows();
	InitMenus();
	TEInit();
	InitDialogs(0L);
	InitCursor();
	randSeed=TickCount()%4000;
	gHasGestalt = MyTrapAvailable(_Gestalt);
}


short NumToolboxTraps( void ) {
	if (NGetTrapAddress(_InitGraf, ToolTrap) ==
			NGetTrapAddress(0xAA6E, ToolTrap))
		return(0x0200);
	else
		return(0x0400);
}

TrapType GetTrapType(short theTrap) {

	if ((theTrap & TrapMask) > 0)
		return(ToolTrap);
	else
		return(OSTrap);

}

Boolean MyTrapAvailable(short theTrap) {

	TrapType	tType;

	tType = GetTrapType(theTrap);
	if (tType == ToolTrap)
	theTrap = theTrap & 0x07FF;
	if (theTrap >= NumToolboxTraps())
		theTrap = _Unimplemented;

	return (NGetTrapAddress(theTrap, tType) !=
			NGetTrapAddress(_Unimplemented, ToolTrap));
}

Boolean WNEAvailable(void) {
	return MyTrapAvailable(_WaitNextEvent);
}


Boolean GestaltAvailable(void) {
	return MyTrapAvailable(_Gestalt);
}

void PlayGame(int nplayers) {
	DisableItem(GetMHandle(rFileMenu),rmNewOne);
	DisableItem(GetMHandle(rFileMenu),rmNewTwo);
	DisableItem(GetMHandle(rGameMenu),rmResume);

	RunGame(nplayers);
	FadeDown(20);

	EnableItem(GetMHandle(rFileMenu),rmNewOne);
	EnableItem(GetMHandle(rFileMenu),rmNewTwo);

	if (SuspendedGameAvailable()) EnableItem(GetMHandle(rGameMenu),rmResume);
	InvalRect(&gGameRect);
	mainWidgets.isFaded = TRUE;
}

int GetCheatLevel(void);
void PlayCheatGame(int nplayers) {
	DisableItem(GetMHandle(rFileMenu),rmNewOne);
	DisableItem(GetMHandle(rFileMenu),rmNewTwo);
	DisableItem(GetMHandle(rGameMenu),rmResume);
	DisableItem(GetMHandle(rGameMenu),rmSuspend);

	RunCheatGame(nplayers, GetCheatLevel());
	FadeDown(20);

	EnableItem(GetMHandle(rFileMenu),rmNewOne);
	EnableItem(GetMHandle(rFileMenu),rmNewTwo);

	if (SuspendedGameAvailable()) EnableItem(GetMHandle(rGameMenu),rmResume);
	InvalRect(&gGameRect);
	mainWidgets.isFaded = TRUE;
}


void SetupMenus() {
	Handle 				theMBar;
	MenuHandle			appleMenu;
	
	theMBar=GetNewMBar(rMainMBAR);

	SetMenuBar(theMBar);
	appleMenu=GetMHandle(rAppleMenu);
	AddResMenu(appleMenu,'DRVR');

	DisableItem(GetMHandle(rGameMenu),rmSuspend);
	if (SuspendedGameAvailable()) EnableItem(GetMHandle(rGameMenu),rmResume);
	else DisableItem(GetMHandle(rGameMenu),rmResume);

	DrawMenuBar();	
}

void RGBFore(unsigned int red,unsigned int green,unsigned int blue) {
	RGBColor 	c;
	long 		t;
	
	if (red && green && blue && (mainWidgets.brightenColors)) {
		t = red + 20000L;
		if (t>65535) t = 65535;
		c.red = t;

		t = blue+20000L;
		if (t>65535) t = 65535;
		c.blue = t;

		t = green+20000L;
		if (t>65535) t = 65535;
		c.green = t;
	} else {
		c.red=red;
		c.blue=blue;
		c.green=green;
	}
	RGBForeColor(&c);
}

HandleAbout()
{
	short itemHit;
	DialogPtr theAbout;

	GrafPtr oldPort;
	GetPort(&oldPort);
	theAbout=GetNewDialog(rAboutDlog,0L,(WindowPtr)-1L);
	SetPort(theAbout);

	RGBFore(65535,65535,65535);
	ModalDialog(0L,&itemHit);
	DisposDialog(theAbout);

	RGBFore(0,0,0);
	SetPort(oldPort);
}

HandleAppleMenu(int whichItem)
{
	Str255 daName;
	if (whichItem==rmAboutAsterax) HandleAbout();
	else {
		GetItem( GetMHandle(rAppleMenu), whichItem, daName );
		OpenDeskAcc( daName );
	}
}

HandleFileMenu(int whichItem) {
	switch (whichItem) {
		case rmNewOne:PlayGame(1);
					break;
		case rmNewTwo: PlayGame(2);
					break;
		case rmQuit:TerminateProgram();break;
	}
}

void HandleGameMenu(int whichItem) {
	switch (whichItem) {
		case 1: ChangePrefVolumeLevel(1);break;
		case 2: ChangePrefVolumeLevel(-1);break;
		case rmSuspend:SuspendGame();
					if (SuspendedGameAvailable()) EnableItem(GetMHandle(rGameMenu),rmResume);
					else DisableItem(GetMHandle(rGameMenu),rmResume);		
					break;		
		case rmResume:ResumeGame();
					if (SuspendedGameAvailable()) EnableItem(GetMHandle(rGameMenu),rmResume);
					else DisableItem(GetMHandle(rGameMenu),rmResume);
					break;
		case rmCheat:mainWidgets.screenMode = cheatMode;
					InvalRect(&gGameRect);
				break;
	}
	

}

void HandleHackMenu(int whichItem) {
	switch (whichItem) {
		case 1: SetPrefDifficulty(kReallyExpertDif);
				ResetHighScoreMarks();	
				break;
		case 2: syncToClock = !syncToClock;
				CheckItem(GetMHandle(rHackMenu),2,syncToClock);
				break;
		case 3: alienProximityWarning = !alienProximityWarning;
				CheckItem(GetMHandle(rHackMenu),3,alienProximityWarning);
	}
}

Boolean HandleMenu(long menuData)
{
	Boolean retVal = TRUE;
	switch (HiWord(menuData))
	{	
		case rAppleMenu:HandleAppleMenu(LoWord(menuData));retVal = FALSE;break;
		case rFileMenu:	HandleFileMenu(LoWord(menuData));retVal = FALSE;break;
		case rGameMenu: HandleGameMenu(LoWord(menuData));retVal = FALSE;break;
		case rHackMenu: HandleHackMenu(LoWord(menuData));retVal = FALSE;break;
	}
	HiliteMenu(0);
	return retVal;
}

// returns TRUE if the application has been hidden
Boolean HandleMenuMouse(EventRecord theEvent)
{
	long retVal;

	retVal=MenuSelect(theEvent.where);
	return HandleMenu(retVal);
}

void HandleMouseEvent(EventRecord theEvent)
{
	WindowPtr whichWindow;
	int part,i;
	Rect r;
	Boolean redrawMbar= FALSE;
	
	if (theEvent.where.v < 20) {
		SATShowMBar();	
		redrawMbar = TRUE;
	}
	switch (part = FindWindow(theEvent.where,&whichWindow))
	{
		case inMenuBar:if (HandleMenuMouse(theEvent)) redrawMbar = FALSE;break;
		case inSysWindow:SystemClick(&theEvent,whichWindow);break;		
		case inContent:
			GlobalToLocal(&theEvent.where);
			switch (mainWidgets.screenMode) {
				case mainScreenMode:
					if (PtInRect(theEvent.where,&mainWidgets.highScoreInfoRect)) {
						fullHighScores = 1-fullHighScores;
						r = gGameRect;
						r.right/=2;
						InvalRect(&r);	
					} else if (PtInRect(theEvent.where,&mainWidgets.beginOneRect)) {
						if (TrackMouseInButton(mainWidgets.redOut,mainWidgets.redIn,
										mainWidgets.beginOneRect)) PlayGame(1);
					} else if (PtInRect(theEvent.where,&mainWidgets.beginTwoRect)) {
						if (TrackMouseInButton(mainWidgets.redOut,mainWidgets.redIn,
										mainWidgets.beginTwoRect)) PlayGame(2);
					} else if (PtInRect(theEvent.where,&mainWidgets.keyRect)) {
						if (TrackMouseInButton(mainWidgets.blueOut,mainWidgets.blueIn,
										mainWidgets.keyRect)) {										
							mainWidgets.screenMode = getKeysMode;
							mainWidgets.setKeySelected = -1;
							InvalRect(&gGameRect);
						}
					} else if (PtInRect(theEvent.where,&mainWidgets.prefRect)) {
						if (TrackMouseInButton(mainWidgets.blueOut,mainWidgets.blueIn,
										mainWidgets.prefRect)) {										
							mainWidgets.screenMode = prefsMode;
							InvalRect(&gGameRect);
						}
					} else if (PtInRect(theEvent.where,&mainWidgets.aboutRect)) {	
						if (TrackMouseInButton(mainWidgets.greenOut,mainWidgets.greenIn,
										mainWidgets.aboutRect)) {										
						 	mainWidgets.screenMode = aboutMode;
						 	mainWidgets.aboutScreenNum = 1;
							InvalRect(&gGameRect);
							FadeDown(20);
							mainWidgets.isFaded = TRUE;												
						}
					} else if (PtInRect(theEvent.where,&mainWidgets.quitRect)) {		
						if (TrackMouseInButton(mainWidgets.redOut,mainWidgets.redIn,
										mainWidgets.quitRect)) {										
							gDone = 1;
						}
					} else if (PtInRect(theEvent.where,&mainWidgets.volumeUpRect)) {
						if (TrackMouseInButton(mainWidgets.arrowUp,mainWidgets.arrowUpLit,
										mainWidgets.volumeUpRect)) {
								ChangePrefVolumeLevel(1);
								SetRect(&r, mainWidgets.volumeUpRect.left+16, mainWidgets.volumeUpRect.top,
											mainWidgets.volumeUpRect.left+30,mainWidgets.volumeUpRect.bottom+5);
								InvalRect(&r);			
							}
					} else if (PtInRect(theEvent.where,&mainWidgets.volumeDownRect)) {
						if (TrackMouseInButton(mainWidgets.arrowDown,mainWidgets.arrowDownLit,
										mainWidgets.volumeDownRect)) {
								ChangePrefVolumeLevel(-1);
								SetRect(&r, mainWidgets.volumeUpRect.left+16, mainWidgets.volumeUpRect.top,
											mainWidgets.volumeUpRect.left+30,mainWidgets.volumeUpRect.bottom+5);
								InvalRect(&r);
							}
					} else if (PtInRect(theEvent.where,&mainWidgets.diffUpRect)) {
						if (TrackMouseInButton(mainWidgets.arrowUp,mainWidgets.arrowUpLit,
										mainWidgets.diffUpRect)) {
								if (PrefDifficulty()+1 <= kExpertDif || 
									 	(allowHeinousDifficulty && PrefDifficulty()+1 <= 
									 	kReallyExpertDif)) {
									 	
									SetPrefDifficulty(PrefDifficulty()+1);
									ResetHighScoreMarks();	
									SetRect(&r, mainWidgets.diffUpRect.right,
												mainWidgets.diffUpRect.top,
												mainWidgets.diffDownRect.left,
												mainWidgets.diffDownRect.bottom);
									InvalRect(&r);
									InvalRect(&mainWidgets.highScoreRect);
							}
						}
					} else if (PtInRect(theEvent.where,&mainWidgets.diffDownRect)) {
						if (TrackMouseInButton(mainWidgets.arrowDown,mainWidgets.arrowDownLit,
										mainWidgets.diffDownRect)) {
								if (PrefDifficulty()-1 >= kBeginnerDif) {									 	
									SetPrefDifficulty(PrefDifficulty()-1);
									ResetHighScoreMarks();	
									SetRect(&r, mainWidgets.diffUpRect.right,
												mainWidgets.diffUpRect.top,
												mainWidgets.diffDownRect.left,
												mainWidgets.diffDownRect.bottom);
									InvalRect(&r);
									InvalRect(&mainWidgets.highScoreRect);
							}
						}
					}					
					break;			
				case prefsMode:
					if (PtInRect(theEvent.where,&mainWidgets.doneRect)) {
						if (TrackMouseInButton(mainWidgets.doneOut,mainWidgets.doneIn,
										mainWidgets.doneRect)) {														
							mainWidgets.screenMode= mainScreenMode;
							InvalRect(&gGameRect);
							UpdatePrefsFile();
							FadeDown(20);
							mainWidgets.isFaded = TRUE;
						}
					} else if (PtInRect(theEvent.where,&mainWidgets.prefScreenRect)) {
						if (TrackMouseInButton(mainWidgets.redOut,mainWidgets.redIn,
										mainWidgets.prefScreenRect)) {
							if (DoScreenPrefsDialog(FALSE)) {
								Str255	str;
								RGBFore(0,0,0);
								r = gGameRect;
								InsetRect(&r,-500,-500);
								PaintRect(&r);
								GetIndString(str,rGeneralStrings,rNeedToRestart);
								
								ParamText(str,0L,0L,0L);
								Alert(rDefaultAlert,0L);
								TerminateProgram();
							}
							RGBFore(0,0,0);
							PaintRect(&gGameRect);
						}
					} else if (PtInRect(theEvent.where,&mainWidgets.prefDrawRect)) {
						TogglePrefFastDraw();
						InvalRect(&mainWidgets.prefDrawRect);
					} else if (PtInRect(theEvent.where,&mainWidgets.prefSpeedRect)) {
						TogglePrefSpeed();
						InvalRect(&mainWidgets.prefSpeedRect);
					} else if (PtInRect(theEvent.where,&mainWidgets.prefRulesRect)) {
						TogglePrefNewRules();
						InvalRect(&mainWidgets.prefRulesRect);
					}					
					break;
				case aboutMode:
					if (PtInRect(theEvent.where,&mainWidgets.doneRect)) {
						if (TrackMouseInButton(mainWidgets.doneOut,mainWidgets.doneIn,
										mainWidgets.doneRect)) {														
							mainWidgets.screenMode= mainScreenMode;			
							InvalRect(&gGameRect);
							FadeDown(20);
							mainWidgets.isFaded = TRUE;
						}
					} else if (PtInRect(theEvent.where,&mainWidgets.nextRect)) {
						if (TrackMouseInButton(mainWidgets.nextOut,mainWidgets.nextIn,
										mainWidgets.nextRect)) {														
							mainWidgets.aboutScreenNum++;
							InvalRect(&gGameRect);
							if (mainWidgets.aboutScreenNum == kLastAbout) {
								mainWidgets.screenMode = mainScreenMode;
							}
							FadeDown(20);
							mainWidgets.isFaded = TRUE;
						}
					}
					break;
				case getKeysMode:
					if (PtInRect(theEvent.where,&mainWidgets.doneRect)) {
						if (TrackMouseInButton(mainWidgets.doneOut,mainWidgets.doneIn,
										mainWidgets.doneRect)) {														
							mainWidgets.screenMode= mainScreenMode;
							InvalRect(&gGameRect);
							FadeDown(20);
		 					mainWidgets.isFaded = TRUE;
		 				}
					} else if (PtInRect(theEvent.where,&mainWidgets.p1keyRect)) {
						InvalKeyRect();
						mainWidgets.setKeySelected =
							(theEvent.where.v - mainWidgets.p1keyRect.top) * keyNum /
							(mainWidgets.p1keyRect.bottom - mainWidgets.p1keyRect.top);
						InvalKeyRect();

						DrawPlayerKeyButtons(mainWidgets.p1keyRect, 1, (mainWidgets.setKeySelected>=0 && mainWidgets.setKeySelected < keyNum)?mainWidgets.setKeySelected:-1);
						while (StillDown()) ;
					} else if (PtInRect(theEvent.where,&mainWidgets.p2keyRect)) {
						InvalKeyRect();
						mainWidgets.setKeySelected =
							(theEvent.where.v - mainWidgets.p2keyRect.top) * keyNum /
							(mainWidgets.p2keyRect.bottom - mainWidgets.p2keyRect.top) + 6;
						InvalKeyRect();
						DrawPlayerKeyButtons(mainWidgets.p2keyRect, 2, (mainWidgets.setKeySelected>=0 && mainWidgets.setKeySelected >= keyNum)?mainWidgets.setKeySelected-keyNum:-1);
						while (StillDown()) ;
					} else {
						mainWidgets.setKeySelected = -1;
						InvalKeyRect();
					}
					break;
				case cheatMode:
					if (HandleCheatModeClick(theEvent.where)) {
						mainWidgets.screenMode = mainScreenMode;
						InvalRect(&gGameRect);
					}
					break;
				}
	}
	if (redrawMbar) {
		SATHideMBar(NULL);
		PeekOffscreen();
		InvalRect(&gGameRect);
	}
}	

InvalKeyRect() {
	Rect r,t;
	int i;
	if (mainWidgets.setKeySelected >= 0) {
		if (mainWidgets.setKeySelected >= keyNum) r = mainWidgets.p2keyRect;
		else r= mainWidgets.p1keyRect;
		i = mainWidgets.setKeySelected % keyNum;
		SetRect(&t,r.left,r.top + (r.bottom - r.top) * i / keyNum,r.right,
						  r.top + (r.bottom - r.top) * (i+1)/keyNum);
		InvalRect(&t);
	}
}

void HandleKey(EventRecord theEvent)
{
	int keyCode,charCode;
	long menuRet,outCount;	
	charCode = BitAnd(theEvent.message,charCodeMask);
	keyCode = (BitAnd(theEvent.message,keyCodeMask)>>8);
	
	if (BitAnd(cmdKey,theEvent.modifiers)) {
		menuRet = MenuKey(charCode);
		HandleMenu(menuRet);
		return;
	}
}

void HandleUpdate(EventRecord theEvent)
{
	GrafPtr oldPort;
	WindowPtr myWind = (WindowPtr)theEvent.message;
	int i;

	if (!gActive) return;
	SATDepthChangeTest();
	GetPort(&oldPort);
	SetPort(myWind);
	BeginUpdate(myWind);

	// the only window that exists is the main game window...
	// need to redraw the intro screen.

	DrawEverything(&myWind->portRect);
	EndUpdate(myWind);
	SetPort(oldPort);
	if (mainWidgets.isFaded) {
		FadeUp(20);
		mainWidgets.isFaded = FALSE;
	}

	if (mainWidgets.setKeySelected >= 0) GetTheKey();
}

GetTheKey() {
	 unsigned char km[16];
	 int i,j;
	 char keyCode;
	 Boolean foundKey=FALSE;
	while (mainWidgets.setKeySelected >= 0) {
		GetKeys( (long *) km);
		keyCode = 0;
		for (i=0;i<16;i++) {
			if (km[i]) {
				j=0; 
				while (km[i]) {
					j++;
					km[i] /= 2;
				}
				keyCode = i*8 + j - 1;
				foundKey = TRUE;
				break;
			}
		}
		
		if (foundKey) {			
			if (mainWidgets.setKeySelected >= keyNum) {
				ChangeKey(keyCode, 2, mainWidgets.setKeySelected-keyNum);
				InvalKeyRect();
			} else {
				ChangeKey(keyCode, 1, mainWidgets.setKeySelected);
				InvalKeyRect();
			}
			mainWidgets.setKeySelected = -1;
		}
		if (Button()) {
			break;
		}
	}
}

DrawEverything(Rect *r) {
	RGBColor c;
	c.red=c.green=c.blue=0;
	RGBForeColor(&c);
	PaintRect(r);
	switch (mainWidgets.screenMode) {
		case mainScreenMode:DrawMainScreen();break;
		case aboutMode:DrawAbout(mainWidgets.aboutScreenNum,
								 mainWidgets.nextOut,
								 mainWidgets.doneOut,
								 &mainWidgets.nextRect,
								 &mainWidgets.doneRect);break;	
		case getKeysMode:DrawGetKeys();break;
		case prefsMode:DrawPreferences();break;
		case cheatMode:DrawCheat();break;
	}
}

DrawPreferences() {
	PicHandle 	hPic;
	Rect 		r;
	Point 		p;
	int 		sphereHeight;
	Str255		str;

	hPic = GetPicture(rIntroPicID);
	r = (*hPic)->picFrame;
	OffsetRect(&r,10,20);
	DrawPicture(hPic,&r);
	
	OffsetRect(&r,0,(*hPic)->picFrame.bottom - (*hPic)->picFrame.top +10);
	r.right = r.left + (*mainWidgets.prefPict)->picFrame.right;
	r.bottom = r.top + (*mainWidgets.prefPict)->picFrame.bottom;
	DrawPicture(mainWidgets.prefPict,&r);
	
	RGBFore(65535,65535,65535);
	TextSize(12);

	// prefs are: fastDraw, screen size, volume?  difficulty?
	sphereHeight = (*mainWidgets.prefPict)->picFrame.bottom - (*mainWidgets.prefPict)->picFrame.top;
	OffsetRect(&r,0,sphereHeight +30);
	r.right = r.left + (*mainWidgets.redOut)->picFrame.right;
	r.bottom = r.top + (*mainWidgets.redOut)->picFrame.bottom;
	DrawPicture(mainWidgets.redOut,&r);
	MoveTo(r.right+10,r.bottom - 10);
	GetIndString(str, rGeneralStrings, rSetScreenSize);
	DrawString(str);
	mainWidgets.prefScreenRect = r;

	OffsetRect(&r,0, sphereHeight +30);
	r.right = r.left + (*mainWidgets.redOut)->picFrame.right;
	r.bottom = r.top + (*mainWidgets.redOut)->picFrame.bottom;
	if (PrefFastDraw()) DrawPicture(mainWidgets.redOut,&r);
	else DrawPicture(mainWidgets.redIn,&r);
	MoveTo(r.right+10,r.bottom - 10);
	GetIndString(str, rGeneralStrings, rSlowerButMoreCompatible);
	RGBFore(65535,65535,65535);
	DrawString(str);
	mainWidgets.prefDrawRect = r;

	OffsetRect(&r,0, sphereHeight +30);
	r.right = r.left + (*mainWidgets.redOut)->picFrame.right;
	r.bottom = r.top + (*mainWidgets.redOut)->picFrame.bottom;
	if (GetPrefSpeed()==2) DrawPicture(mainWidgets.redOut,&r);
	else DrawPicture(mainWidgets.redIn,&r);
	MoveTo(r.right+10,r.bottom - 10);
	GetIndString(str, rGeneralStrings, rDoubleSpeed);
	DrawString(str);
	mainWidgets.prefSpeedRect = r;

	OffsetRect(&r,0, sphereHeight +30);
	r.right = r.left + (*mainWidgets.redOut)->picFrame.right;
	r.bottom = r.top + (*mainWidgets.redOut)->picFrame.bottom;
	if (GetPrefNewRules()) DrawPicture(mainWidgets.redIn,&r);
	else DrawPicture(mainWidgets.redOut,&r);
	MoveTo(r.right+10,r.bottom - 10);
	GetIndString(str, rGeneralStrings, rNewGameParameters);
	DrawString(str);
	mainWidgets.prefRulesRect = r;

	TextSize(9);
	TextFace(bold);
	RGBFore(0,0,0);
	DrawDoneButton(mainWidgets.doneOut, &mainWidgets.doneRect);
}

DrawGetKeys() {
	PicHandle thePic;
	Rect r, pr, t, kr, box;
	int height, i;
	Str255 buf;
	RGBColor c;

	c.red=32767;
	c.blue =c.green=0;
	r=gGameRect;
	InsetRect(&r,36,36 );
	DrawFuzzyRoundRect(&r,c);

	thePic = GetPicture(rSmallLogo);
	SetRect(&pr,r.left + 10,r.top + 3,r.left+10+(*thePic)->picFrame.right,r.top+3+(*thePic)->picFrame.bottom);
	DrawPicture(thePic,&pr);

	TextSize(16);	
	TextFace(italic | bold);
	MoveTo(pr.right+20,pr.bottom-2);
	RGBFore(65535,0,0);

	GetIndString(buf, rGeneralStrings, rKeySetup);
	DrawString(buf);

	RGBFore(65535,65535,65535);
	MoveTo(pr.left, pr.bottom + 20);
	TextFace(bold);
	TextSize(10);	
	RGBFore(0,65535,0);
	DrawRsrcListString(rKeyInfoList, 1);
	
	RGBFore(0,65535,65535);
	height = (kKeyRectBottom - kKeyRectTop);
	for (i=2;i<=7;i++) {
		GetIndString(buf,rKeyInfoList,i);
		MoveTo(kP1KeyRectLeft - 10 - StringWidth(buf), 
										kKeyRectTop + height*(i-1)/keyNum - 6);
		DrawString(buf);
	}
	
	MoveTo(kP1KeyRectLeft+15, kKeyRectTop - 5);
	GetIndString(buf, rGeneralStrings, rPlayerOne);
	DrawString(buf);

	MoveTo(kP2KeyRectLeft+15, kKeyRectTop - 5);
	GetIndString(buf, rGeneralStrings, rPlayerTwo);
	DrawString(buf);

	TextFace(0);
	TextFont(9);
	SetRect(&r, r.left+20,kKeyRectBottom+15, r.right - 20, r.bottom - 20);
	RGBFore(65535,65535,0);
	GetIndString(buf, rKeyInfoList, 8);
	DrawStringInRect(buf, &r);
	
	RGBFore(65535,65535,65535);
	SetRect(&mainWidgets.p1keyRect,kP1KeyRectLeft,kKeyRectTop,kP1KeyRectRight,kKeyRectBottom);
	SetRect(&mainWidgets.p2keyRect,kP2KeyRectLeft,kKeyRectTop,kP2KeyRectRight,kKeyRectBottom);
	OffsetRect(&mainWidgets.p1keyRect,15,0);
	OffsetRect(&mainWidgets.p2keyRect,15,0);

	TextFont(geneva);
	TextSize(10);
	TextFace(bold);
	
	DrawPlayerKeyButtons(mainWidgets.p1keyRect, 1, (mainWidgets.setKeySelected>=0 && mainWidgets.setKeySelected < keyNum)?mainWidgets.setKeySelected:-1);
	DrawPlayerKeyButtons(mainWidgets.p2keyRect, 2, (mainWidgets.setKeySelected>=0 && mainWidgets.setKeySelected >= keyNum)?mainWidgets.setKeySelected-keyNum:-1);
	DrawDoneButton(mainWidgets.doneOut, &mainWidgets.doneRect);

}

DrawPlayerKeyButtons(Rect r, int pnum, int select) {	
	int i, height;
	RGBColor boxWhite, selColor;
	Rect kr;
	Str31 buf;

	boxWhite.red = boxWhite.green = boxWhite.blue = 65535;
	selColor.red = selColor.blue = 0;
	selColor.green = 65535;
	for (i=0;i<keyNum;i++) {
		kr = r;
		height = kr.bottom - kr.top;
		kr.top = r.top + i*height/keyNum;
		kr.bottom = r.top + (i+1)*height/keyNum;
		GetIndString(buf,rKeyCodesStr,GetPlayerKey(pnum,i)+1);
		MoveTo(kr.left + (kr.right-kr.left)/2 - StringWidth(buf)/2,kr.top + (kr.bottom - kr.top)/2 + 6);
		DrawString(buf);
		InsetRect(&kr,4,2);
		if (select == i) DrawFuzzyRoundRect(&kr,selColor);
		else DrawFuzzyRoundRect(&kr,boxWhite);
		RGBForeColor(&boxWhite);
	}
}

void HandleActivate(EventRecord theEvent)
{	
	if (theEvent.modifiers & activeFlag) {
		SATHideMBar(NULL);
		PeekOffscreen();
		InvalRect(&gGameRect);	
	}
}

void HandleOSEvent(EventRecord theEvent) {
	if (((theEvent.what & osEvtMessageMask) >> 24) & suspendResumeMessage) {
		if ((theEvent.message) & resumeFlag) {
			ShowWindow(gSAT.wind);
			gActive = TRUE;
		} else {
			HideWindow(gSAT.wind);
			gActive = FALSE;
		}
	}
}


void EventLoop() {
	EventRecord theEvent;
	WaitNextEvent(everyEvent,&theEvent,10,0);
	switch (theEvent.what)
	{
		case mouseDown:HandleMouseEvent(theEvent);break;
		case keyDown:case autoKey:HandleKey(theEvent);break;
		case updateEvt:HandleUpdate(theEvent);break;
		case activateEvt:HandleActivate(theEvent);break;
		case osEvt:HandleOSEvent(theEvent);break;
	}
}

DrawIntroScreen() {
	PicHandle hPic, mainPic;
	Rect r;

	hPic = GetPicture(rIntroPicID);
	r = (*hPic)->picFrame;
	SetupMainWidgets(&r);
	OffsetRect(&r,(gGameRect.right - (r.right-r.left))/2,0);
	DrawPicture(hPic,&r);	

	mainPic = GetPicture(128);
	r = (*mainPic)->picFrame;
	OffsetRect(&r,(gGameRect.right - (r.right-r.left))/2,(*hPic)->picFrame.bottom);
	DrawPicture(mainPic,&r);	
	
	RGBFore(65535,65535,65535);
//	TextSize(36);
//	MoveTo(r.left + (r.right - r.left)/2 - StringWidth("\pBeta Test Version 3.0")/2,r.bottom + 50);
//	DrawString("\pBeta Test Version 3.0");

	TextSize(9);
	ReleaseResource((Handle)hPic);
	ReleaseResource((Handle)mainPic);
}
	
SetupMainWidgets(Rect *titleBox) {
	int 		top,width, width2, vOffset;
	Point 		base;
	Str255		buf;
	mainWidgets.moreHighscoreInfoRgn = NewRgn();

	TextSize(9);
	TextFont(geneva);
	TextFace(bold);
	
	OpenRgn();
	top = 57;
	base.h = gGameRect.left + (gGameRect.right - gGameRect.left)/2 - 80;
	base.v = titleBox->bottom;
	MoveTo(base.h, base.v + top);
	LineTo(base.h + 5,base.v+top+5);
	LineTo(base.h, base.v + top+10);
	LineTo(base.h, base.v+top);
	CloseRgn(mainWidgets.moreHighscoreInfoRgn);

	mainWidgets.moreHighscoreInfoDownRgn = NewRgn();
	OpenRgn();
	MoveTo(base.h-5, base.v + top);
	LineTo(base.h+5,base.v+top);
	LineTo(base.h, base.v + top+5);
	LineTo(base.h-5, base.v+top);
	CloseRgn(mainWidgets.moreHighscoreInfoDownRgn);

	SetRect(&mainWidgets.highScoreInfoRect,base.h-5,base.v,base.h+5,base.v+top+10);
//	titleBox->right - 16,titleBox->bottom + top - 10, 
//										titleBox->right + 19, titleBox->bottom + top + 19);

	mainWidgets.beginOnePict = GetPicture(rBeginOnePict);
	mainWidgets.beginTwoPict = GetPicture(rBeginTwoPict);
	mainWidgets.aboutPict = GetPicture(rAboutPict);
	mainWidgets.prefPict = GetPicture(rPrefsPict);
	mainWidgets.keyPict = GetPicture(rKeysPict);
	mainWidgets.quitPict = GetPicture(rQuitPict);

	mainWidgets.redIn = GetPicture(rRedInPict);
	mainWidgets.redOut = GetPicture(rRedOutPict);
	mainWidgets.greenIn = GetPicture(rGreenInPict);
	mainWidgets.greenOut = GetPicture(rGreenOutPict);
	mainWidgets.blueIn = GetPicture(rBlueInPict);
	mainWidgets.blueOut = GetPicture(rBlueOutPict);

	mainWidgets.doneIn = GetPicture(rDoneInPict);
	mainWidgets.doneOut = GetPicture(rDoneOutPict);
	mainWidgets.nextIn = GetPicture(rNextInPict);
	mainWidgets.nextOut = GetPicture(rNextOutPict);

	mainWidgets.arrowUp = GetPicture(rArrowUpPict);
	mainWidgets.arrowUpLit = GetPicture(rArrowUpLitPict);
	mainWidgets.arrowDown = GetPicture(rArrowDownPict);
	mainWidgets.arrowDownLit = GetPicture(rArrowDownLitPict);


	width = gGameRect.right /2 -50;
	if (gGameRect.top - gGameRect.bottom > 400) vOffset = 48;
	else vOffset = 40;

	SetRect(&mainWidgets.beginOneRect,gGameRect.right/2 + 10,gGameRect.top + vOffset-18,gGameRect.right/2 + 40,gGameRect.top + vOffset-18+40);

	SetRect(&mainWidgets.beginTwoRect,gGameRect.right/2 + 10,mainWidgets.beginOneRect.bottom+8,gGameRect.right/2 + 40,mainWidgets.beginOneRect.bottom+vOffset);
	SetRect(&mainWidgets.aboutRect,   gGameRect.right/2 + 10,mainWidgets.beginTwoRect.bottom +8,gGameRect.right/2 + 40,mainWidgets.beginTwoRect.bottom +vOffset);
	SetRect(&mainWidgets.prefRect,    gGameRect.right/2 + 10,mainWidgets.aboutRect.bottom +8,gGameRect.right/2 + 40,mainWidgets.aboutRect.bottom +vOffset);
	SetRect(&mainWidgets.keyRect,     gGameRect.right/2 + 10,mainWidgets.prefRect.bottom +8,gGameRect.right/2 + 40,mainWidgets.prefRect.bottom +vOffset);
	SetRect(&mainWidgets.quitRect,    gGameRect.right/2 + 10,gGameRect.bottom - 60,gGameRect.right/2 + 40,gGameRect.bottom - 30);									  

	GetIndString(buf, rGeneralStrings, rDifficulty);
	width = StringWidth(buf) + 10;

	GetIndString(buf, rGeneralStrings, rBeginner);
	width2 = StringWidth(buf);
	SetRect(&mainWidgets.volumeUpRect,gGameRect.right/2 + width,
											mainWidgets.keyRect.bottom + 26,0,0);
	mainWidgets.volumeUpRect.right = mainWidgets.volumeUpRect.left + 
											(*mainWidgets.arrowUp)->picFrame.right;
	mainWidgets.volumeUpRect.bottom = mainWidgets.volumeUpRect.top + 
											(*mainWidgets.arrowUp)->picFrame.bottom;
	mainWidgets.volumeDownRect = mainWidgets.volumeUpRect;
	OffsetRect(&mainWidgets.volumeDownRect,10+(*mainWidgets.arrowUp)->picFrame.right,0);

	SetRect(&mainWidgets.diffUpRect, gGameRect.right/2 + width, mainWidgets.keyRect.bottom+ 4,0,0);
	mainWidgets.diffUpRect.right = mainWidgets.diffUpRect.left + (*mainWidgets.arrowUp)->picFrame.right;
	mainWidgets.diffUpRect.bottom = mainWidgets.diffUpRect.top + 
											(*mainWidgets.arrowUp)->picFrame.bottom;
	mainWidgets.diffDownRect = mainWidgets.diffUpRect;
	OffsetRect(&mainWidgets.diffDownRect,width2+5+(*mainWidgets.arrowUp)->picFrame.right,0);
}

DrawMainScreen() {
	PicHandle 		hPic;
	Rect 			r;
	RGBColor 		c;
	Point 			p;
	Str15 			buf;
	Str255			bigBuf;
	short 			theVolume;

	hPic = GetPicture(rIntroPicID);
	r = (*hPic)->picFrame;
	OffsetRect(&r,10,20);
	DrawPicture(hPic,&r);
	
	SetRect(&mainWidgets.highScoreRect, 0, r.bottom+20, gGameRect.left + (gGameRect.right- gGameRect.left)/2,gGameRect.bottom);
	DrawHighScores(r.bottom+20,fullHighScores);
	RGBFore(30000,30000,50000);

	if (fullHighScores) PaintRgn(mainWidgets.moreHighscoreInfoDownRgn);
	else PaintRgn(mainWidgets.moreHighscoreInfoRgn);

	RGBFore(50000,50000,50000);

	if (fullHighScores) FrameRgn(mainWidgets.moreHighscoreInfoDownRgn);
	else FrameRgn(mainWidgets.moreHighscoreInfoRgn);

	RGBFore(32767,32767,32767);
	MoveTo((*(mainWidgets.moreHighscoreInfoDownRgn))->rgnBBox.right+4,
		   (*(mainWidgets.moreHighscoreInfoDownRgn))->rgnBBox.bottom+2);
	GetIndString(bigBuf, rGeneralStrings, rMore);
	DrawString(bigBuf);

	MoveTo(mainWidgets.beginOneRect.left - 10,mainWidgets.beginOneRect.top);
	LineTo(mainWidgets.beginOneRect.left - 10,mainWidgets.quitRect.bottom + 10);

	MoveTo(mainWidgets.quitRect.left - 10,mainWidgets.quitRect.top - 10);
	LineTo(gGameRect.right -10, mainWidgets.quitRect.top - 10);
	
	p.h = mainWidgets.beginOneRect.left;
	p.v = mainWidgets.beginOneRect.top;
	DrawButtonAndLabel(mainWidgets.redOut, mainWidgets.beginOnePict, p);

	p.h = mainWidgets.beginTwoRect.left;
	p.v = mainWidgets.beginTwoRect.top;
	DrawButtonAndLabel(mainWidgets.redOut, mainWidgets.beginTwoPict, p);

	p.h = mainWidgets.quitRect.left;
	p.v = mainWidgets.quitRect.top;
	DrawButtonAndLabel(mainWidgets.redOut, mainWidgets.quitPict, p);

	p.h = mainWidgets.aboutRect.left;
	p.v = mainWidgets.aboutRect.top;
	DrawButtonAndLabel(mainWidgets.greenOut, mainWidgets.aboutPict, p);

	p.h = mainWidgets.prefRect.left;
	p.v = mainWidgets.prefRect.top;
	DrawButtonAndLabel(mainWidgets.blueOut, mainWidgets.prefPict, p);

	p.h = mainWidgets.keyRect.left;
	p.v = mainWidgets.keyRect.top;
	DrawButtonAndLabel(mainWidgets.blueOut, mainWidgets.keyPict, p);

	RGBFore(65535,65535,65535);

	DrawPicture(mainWidgets.arrowUp, &mainWidgets.diffUpRect);
	DrawPicture(mainWidgets.arrowDown, &mainWidgets.diffDownRect);

	MoveTo(mainWidgets.keyRect.left, mainWidgets.keyRect.bottom + 14);


	GetIndString(bigBuf, rGeneralStrings, rDifficulty);
	DrawString(bigBuf);

	MoveTo(mainWidgets.diffUpRect.left + (*mainWidgets.arrowUp)->picFrame.right + 2,
		   mainWidgets.keyRect.bottom+14);
		   
	switch (PrefDifficulty()) {
		case kBeginnerDif:GetIndString(bigBuf, rGeneralStrings, rBeginner);break;
		case kNormalDif:GetIndString(bigBuf, rGeneralStrings, rNormal);break;
		case kExpertDif:GetIndString(bigBuf, rGeneralStrings, rExpert);break;
		case kReallyExpertDif:bigBuf[0] = 0;DrawString("\pHeinous");break;	
	}
	DrawString(bigBuf);

	MoveTo(mainWidgets.keyRect.left, mainWidgets.volumeUpRect.bottom-4);
	if (UseSounds()) {
		GetIndString(bigBuf, rGeneralStrings, rVolume);
		DrawString(bigBuf);
		DrawPicture(mainWidgets.arrowUp, &mainWidgets.volumeUpRect);
		DrawPicture(mainWidgets.arrowDown, &mainWidgets.volumeDownRect);
		MoveTo(mainWidgets.volumeDownRect.left-8,mainWidgets.volumeDownRect.bottom-4);
		
		GetSoundVol(&theVolume);
		NumToString((long)theVolume,buf);
		DrawString(buf);
	} else {
		GetIndString(bigBuf, rGeneralStrings, rNoSoundFile);
		DrawString(bigBuf);
	}
	MoveTo(mainWidgets.keyRect.left, mainWidgets.volumeUpRect.bottom + 16);
	if (!SuspendedGameAvailable()) {
		GetIndString(bigBuf, rGeneralStrings, rSuspendedNotAvailable);
		DrawString(bigBuf);
	} else {
		int sLev, sPlayers;
		GetSuspendedSummary(&sLev, &sPlayers);
		GetIndString(bigBuf, rGeneralStrings, rSuspendedGame);
		DrawString(bigBuf);
		GetIndString(bigBuf, rGeneralStrings, rLevel);
		sLev++;
		DrawString(bigBuf);
		DrawChar(' ');
		DrawInt(sLev);
		DrawString("\p, ");
		DrawInt(sPlayers);DrawChar(' ');
		if (sPlayers == 2) GetIndString(bigBuf, rGeneralStrings, rPlayers);
		else GetIndString(bigBuf, rGeneralStrings, rPlayer);
		DrawString(bigBuf);
	}	

	MoveTo(mainWidgets.quitRect.left - 10,mainWidgets.quitRect.top - 10);
	LineTo(gGameRect.right -10, mainWidgets.quitRect.top - 10);

	RGBFore(0,65535,0);
	MoveTo(mainWidgets.quitRect.left, mainWidgets.quitRect.top - 30);
	GetIndString(bigBuf, rGeneralStrings, rPleaseSupport);
	DrawString(bigBuf);
	MoveTo(mainWidgets.quitRect.left, mainWidgets.quitRect.top - 14);
	GetIndString(bigBuf, rGeneralStrings, rPleaseSupport2);
	DrawString(bigBuf);
	
	RGBFore(0,0,0);
}

DrawFuzzyRoundRect(Rect *r,RGBColor c) {
	Rect t = *r;
	RGBForeColor(&c);
	FrameRoundRect(&t,16,16);
	c.red/=2;c.green/=2;c.blue/=2;
	RGBForeColor(&c);
	InsetRect(&t,-1,-1);
	FrameRoundRect(&t,17,17);
	InsetRect(&t,2,2);
	FrameRoundRect(&t,15,15);
}

DrawPictureCentered(PicHandle pic,Rect *r) {
	Rect t;
	int midh, midv;
	midh = r->left + (r->right - r->left)/2;
	midv = r->top + (r->bottom - r->top)/2;
	SetRect(&t,midh - (*pic)->picFrame.right/2,midv - (*pic)->picFrame.bottom/2,0,0);
	t.right = t.left + (*pic)->picFrame.right;
	t.bottom = t.top + (*pic)->picFrame.bottom;
	DrawPicture(pic,&t);
}

DrawButtonAndLabel(PicHandle button, PicHandle label, Point topLeft) {
	Rect r,t;
	SetRect(&r,topLeft.h,topLeft.v,topLeft.h + (*button)->picFrame.right,topLeft.v + (*button)->picFrame.bottom);
	DrawPicture(button, &r);
	SetRect(&t,r.right+5,topLeft.v,r.right+5+ (*label)->picFrame.right,topLeft.v + (*label)->picFrame.bottom);
	DrawPicture(label, &t);
}

Boolean TrackMouseInButton(PicHandle outPic, PicHandle inPic, Rect r) {
	Point p;
	Rect br;
	Boolean last=FALSE;
	SetRect(&br,r.left,r.top,r.left + (*outPic)->picFrame.right,
								r.top + (*outPic)->picFrame.bottom);
	do {
		GetMouse(&p);
		//GlobalToLocal(&p);
		if (PtInRect(p,&r)) {
			if (!last) {
				DrawPicture(inPic,&br);
			}
			last = TRUE;
		} else {
			if (last) {
				DrawPicture(outPic,&br);
			}
			last = FALSE;
		}
	} while (Button());
	DrawPicture(outPic,&br);
	return last;
}

DrawStatusBar(Rect *r,int cur, int max) {
	Rect t;
	RGBFore(55000,20000,10000);
	SetRect(&t,r->left,r->top,r->left + (r->right - r->left) * cur / max, r->bottom);
	InsetRect(&t,1,1);
	PaintRect(&t);
	RGBFore(65535,40000,40000);
	MoveTo(t.right,t.top);
	LineTo(t.right,t.bottom);
	LineTo(t.left,t.bottom);
}


void LoadSprites() {
	Rect r;
	Str255 buf;
	int i=0,nModules = 19;

	SetRect(&r,gGameRect.left + 100,gGameRect.bottom - 50,
			   gGameRect.right - 100,gGameRect.bottom - 40);
	TextFont(geneva);
	TextSize(9);
	TextFace(bold);

	RGBFore(30000,30000,30000);
	if (mainWidgets.brightenColors) RGBFore(5000,5000,5000);
	PaintRect(&r);

	RGBFore(15000,15000,15000);
	if (mainWidgets.brightenColors) RGBFore(50000,50000,50000);
	MoveTo(r.left,r.bottom);
	LineTo(r.left,r.top);
	LineTo(r.right-1,r.top);

	RGBFore(50000,50000,50000);
	LineTo(r.right-1,r.bottom-1);
	LineTo(r.left,r.bottom-1);

	MoveTo(r.left, r.top - 4);

	GetIndString(buf, rGeneralStrings, rLoadingGraphics);
	DrawString(buf);

	InitPlayerShip();i++;DrawStatusBar(&r,i,nModules);
	InitPlayerShot();i++;DrawStatusBar(&r,i,nModules);
	InitBigAsteroid();i++;DrawStatusBar(&r,i,nModules);
	InitMidAsteroid();i++;DrawStatusBar(&r,i,nModules);
	InitSmallAsteroid();i++;DrawStatusBar(&r,i,nModules);
	InitExplosion();i++;DrawStatusBar(&r,i,nModules);
	InitShieldHit();i++;DrawStatusBar(&r,i,nModules);
	InitCrystal();i++;DrawStatusBar(&r,i,nModules);
	InitMoney();i++;DrawStatusBar(&r,i,nModules);
 	InitStores();i++;DrawStatusBar(&r,i,nModules);
	InitItems();i++;DrawStatusBar(&r,i,nModules);
	InitHilight();i++;DrawStatusBar(&r,i,nModules);
	InitSimpleSaucer();i++;DrawStatusBar(&r,i,nModules);
	InitLeechSaucer();i++;DrawStatusBar(&r,i,nModules);
	InitEnemyShot();i++;DrawStatusBar(&r,i,nModules);
	InitSpiderSaucer();i++;DrawStatusBar(&r,i,nModules);
	InitCrystalGrabber();i++;DrawStatusBar(&r,i,nModules);
	InitMine();i++;DrawStatusBar(&r,i,nModules);
	RGBFore(0,0,0);
	r.top -= 20;
	PaintRect(&r);

}


void HandleTerminalError(int resource, int index, char *replace) {
	Str255 buffer, buf2;
	int i;
	GetIndString(buffer,resource,index);
	if (replace) {
		PtoCstr(buffer);
		i=0;
		*buf2=0;
		while (buffer[i]) {
			if (buffer[i] == '*') {
				strncpy((char *)buf2,(char *)buffer,i);
				buf2[i]=0;
				strcat((char *)buf2,(char *)replace);
				strcat((char *)buf2,(char *)&buffer[i+1]);
				break;
			}
			i++;
		}
		if (*buf2) {
			strcpy((char *)buffer,(char *)buf2);
			CtoPstr((char *)buffer);
		}
	}
	ParamText(buffer,0L,0L,0L);
	Alert(rDefaultAlert,0L);
	ExitToShell();
}

void CheckDepth() {
	GDHandle devH;
	PixMapHandle theMap;
	DialogPtr d;
	OSErr myErr;
	GrafPtr oldPort;
	short itemHit =0,hasMode,item;
	Handle handle;
	Rect itemRect;
	
	SetRect(&itemRect,-30000,-30000,30000,30000);
	devH = GetMaxDevice(&itemRect);
	SetGDevice(devH);	
	
	theMap = (*devH)->gdPMap;
	
	mainWidgets.originalColorDepth = (*theMap)->pixelSize;
	if ((*theMap)->pixelSize > 8 ||  ((*theMap)->pixelSize == 1)) {
		GetPort(&oldPort);
		d = GetNewDialog(rDepthDialog,0L,(WindowPtr)-1L);
		SetPort(d);
		if (HasDepth(devH,8,0,0)) {
			GetDItem(d,4,&item,&handle,&itemRect);
			HiliteControl((ControlHandle)handle,0);
		}	
		if (HasDepth(devH,4,0,0)) {
			GetDItem(d,3,&item,&handle,&itemRect);
			HiliteControl((ControlHandle)handle,0);
		}
		
		while (itemHit < 2) ModalDialog(0L,&itemHit);
		switch (itemHit) {
			case 2:ExitToShell();
			case 3:if (HasDepth(devH,4,0,0)) {
						myErr = SetDepth(devH,4,0,0);
					} else {
						ExitToShell();
					}
					if (myErr) ExitToShell();
					break;
			case 4:if (HasDepth(devH,8,0,0)) {
						myErr = SetDepth(devH,8,0,0);
					} else {
						ExitToShell();
					}
					if (myErr) ExitToShell();
					break;
		}
		DisposDialog(d);	
		SetPort(oldPort);
	}

	if ((*devH)->gdFlags & (1<<gdDevType)) {
		// color
		mainWidgets.brightenColors = FALSE;
	} else {
		// gray-scale
		mainWidgets.brightenColors = TRUE;
	}
}


void CheckSystemVersion() {
	long 		gestaltAnswer;
	OSErr 		myErr;
	int 		foo;
	Str255		buffer;
	
	if (gHasGestalt) {
		myErr = Gestalt(gestaltSystemVersion, &gestaltAnswer);
	}
	if (!gHasGestalt || (gestaltAnswer & 0x0f00) < 0x700) {
		GetIndString(buffer, rGeneralStrings, rNeedSys7);
		ParamText(buffer,0L,0L,0L);
		Alert(rDefaultAlert,0L);
		ExitToShell();
	}
}

void main() {
	WindowPtr 	gameWind;
	Rect 		r;
	int 		i;
	short 		iFileRef;
	Str255		buf;
	OSErr 		err;
/*	// Beta:
	unsigned long secs;
	DateTimeRec dtr;
	*/
	
	ToolBoxInit();
	MaxApplZone();
	MoreMasters();
	MoreMasters();

	CheckSystemVersion();

	// Beta Test:
/*	GetDateTime(&secs);
	Secs2Date(secs,&dtr);
	if (dtr.month > 4 && dtr.day > 15) {
		ParamText("\pThis Beta Test version of Asterax cannot be used after May 15, 1994.",0L,0L,0L);
		Alert(rDefaultAlert,0L);
		ExitToShell();
	}
*/
	// Hack:
	syncToClock = TRUE;
	alienProximityWarning = FALSE;
	
	iFileRef = OpenResFile("\pAsterax Sounds");
	if (iFileRef < 0) {
		DontUseSounds();
		GetIndString(buf, rGeneralStrings, rCouldntFindSound);
		ParamText(buf, 0L,0L,0L);
		Alert(rDefaultAlert+1,0L);
	}

	InitSoundChannels();
	CheckDepth();
	GetSoundVol(&mainWidgets.originalVolume);

	LoadHighScores();
	LoadPreferences();

	CheckForFader();	
	SetupMenus();
	iFileRef = OpenResFile("\pAsterax Data");
	if (iFileRef < 0) {
		GetIndString(buf, rGeneralStrings, rCouldntFindData);
		ParamText(buf, 0L,0L,0L);
		Alert(rDefaultAlert,0L);
		ExitToShell();
	}

	FadeDown(4);
	ConfigureSAT (FALSE, kVPositionSort, kKindCollision, 32);
//	gameWind = InitSAT(0,0,gGameRect.right,gGameRect.bottom);

	r = gGameRect;
	r.top += 15;
	CustomInitSAT (0,0,&r, NULL, NULL, TRUE, FALSE, TRUE, TRUE, TRUE);


	gGameRect.left += gSAT.ox;
	gGameRect.right += gSAT.ox;
	gGameRect.top += gSAT.oy;
	gGameRect.bottom += gSAT.oy;

	ShowWindow(gSAT.wind);
	SelectWindow(gSAT.wind);
	
	SATHideMBar(NULL);
	PeekOffscreen();
	DrawIntroScreen();

	FadeUp(4);
	LoadSprites();

	// while (!Button());
	mainWidgets.setKeySelected = -1;

	fullHighScores = FALSE;
	FadeDown(20);
	mainWidgets.screenMode=  mainScreenMode;
	InvalRect(&gSAT.wind->portRect);
	// DrawEverything(&gSAT.wind->portRect);
	mainWidgets.isFaded = TRUE;	
	while (!gDone) {
		EventLoop();
	}

	TerminateProgram();
}

TerminateProgram() {
	int 		i;
	OSErr 		myErr;
	GDHandle 	devH;
	Rect		itemRect;

	SATShowMBar();
	SATSoundShutup();
	CloseHighScoreFile();
	UpdatePrefsFile();
	ClosePrefsFile();
	ShutUpAllChannels();

	FadeDown(20);
	DisposeWindow(gSAT.wind);

	SetRect(&itemRect,-30000,-30000,30000,30000);
	devH = GetMaxDevice(&itemRect);
	myErr = SetDepth(devH,mainWidgets.originalColorDepth,0,0);

	SetSoundVol(mainWidgets.originalVolume);

	FadeUp(20);
	DeactivateFade();
//	InvalRect(&screenBits.bounds);
	ExitToShell();
}