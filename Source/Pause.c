#include "AsteraxResources.h"
#include "SAT.h"
#include "Sprites.h"
#include "Pause.h"
#include "MainResources.h"

void PauseEventLoop();
void HandlePauseUpdate(EventRecord theEvent);
void HandlePauseMouseEvent(EventRecord theEvent);
void HandlePauseActivate(EventRecord theEvent);
void DrawPauseMessage();
void PauseHandleOSEvent(EventRecord theEvent);

Boolean InCheatMode(void);

/************** Globals **************/
static Boolean		gSignalGameOver;		// used for suspend

Boolean HandlePause(Boolean isMarket) {
	unsigned char km[16];
	Boolean  startCondition,retVal = FALSE;
	Rect r;
	RGBColor c;

	GetKeys( (long *) km);	
	startCondition = (IsPressed(capsLock,km));

	SATShowMBar();
	ShowCursor();
	gSignalGameOver = FALSE;
	
	DrawPauseMessage();
	FlushEvents(everyEvent,0);
	
	DisableItem(GetMHandle(rGameMenu),rmResume);
	if (isMarket && !InCheatMode()) EnableItem(GetMHandle(rGameMenu),rmSuspend);
	
	while (TRUE) {
		PauseEventLoop();
		GetKeys( (long *) km);	
		if (IsPressed(capsLock,km) != startCondition) break;		
		if (IsPressed(escapeKey,km)) {retVal = TRUE;break;}
		if (gSignalGameOver) break;
	}
	HideCursor();
	SATHideMBar(NULL);
	PeekOffscreen();
	FlushEvents(everyEvent,0);
	if (gSignalGameOver) return TRUE;
	return retVal;
}

void PauseEventLoop() {
	EventRecord theEvent;
	WaitNextEvent(everyEvent,&theEvent,10,0);
	switch (theEvent.what)
	{
		case mouseDown:HandlePauseMouseEvent(theEvent);break;
		case keyDown:case autoKey:HandleKey(theEvent);break;
		case updateEvt:HandlePauseUpdate(theEvent);break;
		case activateEvt:HandlePauseActivate(theEvent);break;
		case osEvt:PauseHandleOSEvent(theEvent);break;
	}
}

void PauseHandleOSEvent(EventRecord theEvent) {
	if (((theEvent.what & osEvtMessageMask) >> 24) & suspendResumeMessage) {
		if ((theEvent.message) & resumeFlag) {
			ShowWindow(gSAT.wind);
		} else {
			HideWindow(gSAT.wind);
		}
	}
}

void HandlePauseUpdate(EventRecord theEvent)
{
	GrafPtr oldPort;
	WindowPtr myWind = (WindowPtr)theEvent.message;
	RGBColor c;
	int i;

	SATDepthChangeTest();
	GetPort(&oldPort);
	SetPort(myWind);
	BeginUpdate(myWind);

	// the only window that exists is the main game window...
	// need to redraw the intro screen.

	PeekOffscreen();
	DrawPauseMessage();
	EndUpdate(myWind);
	SetPort(oldPort);
}

void HandlePauseMouseEvent(EventRecord theEvent)
{
	WindowPtr whichWindow;
	int part;
	Boolean redrawMbar= FALSE;
	
	if (theEvent.where.v < 20) {
		SATShowMBar();	
		redrawMbar = TRUE;
	}
	switch (part = FindWindow(theEvent.where,&whichWindow))
	{
		case inMenuBar:HandleMenuMouse(theEvent);break;
		case inSysWindow:SystemClick(&theEvent,whichWindow);break;		
	}
	if (redrawMbar) {
		SATHideMBar(NULL);
	}
}


void HandlePauseActivate(EventRecord theEvent)
{
	if (theEvent.modifiers) {
		// activate event
//		ShowWindow(gSAT.wind);
		InvalRect(&gGameRect);
	} else {
//		HideWindow(gSAT.wind);
		InvalRect(&gGameRect);
	}
}

void DrawPauseMessage() {
	Rect r;
	RGBColor c;
	SetRect(&r,gGameRect.right/2 - 100,
				gGameRect.bottom/2 - 50,
				gGameRect.right/2 + 100,
				gGameRect.bottom/2 + 50);
//	c.red=c.green=c.blue=0;RGBForeColor(&c);
//	PaintRect(&r);
	c.green=c.blue=65535;RGBForeColor(&c);
	MoveTo(r.left+20, r.top + 20);
	DrawString("\pAsterax is Paused");
	MoveTo(r.left+30, r.top + 40);
	DrawString("\pPress Caps Lock to return to game");
	MoveTo(r.left+35, r.top + 52);
	DrawString("\por escape to terminate current game.");
	c.red=c.green=c.blue=0;RGBForeColor(&c);	
}

void PauseSignalGameOver(void) {
	gSignalGameOver = TRUE;
}