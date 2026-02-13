/*
 * Asterax Game Parameter Selection
 *
 */

#include "SAT.h"
#include "AsteraxResources.h"
#include "Sprites.h"
#include "Preferences.h"
#include "gamma.h"
#include "Strings.h"

#define rChooseShipPict	158

#define rShipPic		200

void DrawShipMessage(int ship, Rect *r);


void DrawGameParamBackground(Rect p1r, Rect p2r, int nPlayers) {
	Rect 		r;
	PicHandle	chooseShipPict;
	int 		screenwidth, picwidth;
	Str255		buffer;
	
	chooseShipPict = GetPicture(rChooseShipPict);

	screenwidth = gGameRect.right - gGameRect.left;
	picwidth = (*chooseShipPict)->picFrame.right - (*chooseShipPict)->picFrame.left;

	SetRect(&r,gGameRect.left + screenwidth/2 - picwidth/2,
			   gGameRect.top + 22,
			   gGameRect.left + screenwidth/2 + picwidth/2,
			   gGameRect.top + 22 + (*chooseShipPict)->picFrame.bottom);

	TextFont(geneva); TextSize(9); TextFace(bold);
	RGBFore(0,0,0);
	PaintRect(&gGameRect);
	DrawPicture(chooseShipPict,&r);

	RGBFore(50000,50000,50000);
	FrameRoundRect(&p1r,16,16);
	if (nPlayers==2) FrameRoundRect(&p2r,16,16);

	RGBFore(0,65535,65535);
	GetIndString(buffer, rGeneralStrings, rPlayerOne);
	MoveTo((p1r.right - p1r.left)/2 + p1r.left - StringWidth(buffer)/2,p1r.top + 18);
	DrawString(buffer);
	if (nPlayers == 2) {
		GetIndString(buffer, rGeneralStrings, rPlayerTwo);
		MoveTo((p2r.right - p2r.left)/2 + p2r.left - StringWidth(buffer)/2,p2r.top + 18);
		DrawString(buffer);	
	}

	TextFace(0);
	SetRect(&r,p1r.left + 10, p1r.top + 30, p1r.right - 10, p1r.top + 80);

	GetIndString(buffer, rGeneralStrings, rSelShip);
	DrawStringInRect(buffer,&r);
	if (nPlayers==2) {
		SetRect(&r,p2r.left + 10, p2r.top + 30, p2r.right - 10, p2r.top + 80);
		DrawStringInRect(buffer,&r);
	}
	RGBFore(0,0,0);
}

void DetermineGameParameters(int nPlayers, int *p1type, int *p2type) {
	Rect 		p1r, p2r, r, p1sr, p2sr, p1mr, p2mr;
	GrafPtr		oldPort;
	int 		screenwidth, prectwidth, p1hilight, p2hilight, i, width;
	Boolean		p1sel = FALSE, p2sel = FALSE;
	char		p1left, p1right, p1fire;
	char		p2left, p2right, p2fire;
	SpritePtr	p1sprites[4], p2sprites[4];
	unsigned char km[16];
	PicHandle	chooseShipPict, shipPic;
	long		delay1, delay2, l;

	GetPort(&oldPort);
	
	FadeDown(5);	
	screenwidth = (gGameRect.right - gGameRect.left); 

	chooseShipPict = GetPicture(rChooseShipPict);
	SetRect(&p1r,gGameRect.left + 5,
				 gGameRect.top + (*chooseShipPict)->picFrame.bottom + 30,
				 screenwidth/2 + gGameRect.left - 10,
				 gGameRect.bottom - 40);	
	prectwidth = p1r.right - p1r.left;

	if (nPlayers == 2) {
		p2r = p1r;
		OffsetRect(&p2r, screenwidth/2+10,0);
	} else {
		OffsetRect(&p1r,screenwidth/4,0);
	}

	SetPort(gSAT.backScreen);
	DrawGameParamBackground(p1r,p2r,nPlayers);
	SetPort(oldPort);
	SATBackChanged(&gGameRect);

	p1left = GetPlayerKey(1, kLeftPlayerKey); 
	p1right = GetPlayerKey(1, kRightPlayerKey); 
	p1fire = GetPlayerKey(1, kFirePlayerKey); 

	p2left = GetPlayerKey(2, kLeftPlayerKey); 
	p2right = GetPlayerKey(2, kRightPlayerKey); 
	p2fire = GetPlayerKey(2, kFirePlayerKey); 

	width = p1r.right - p1r.left;
	p1sprites[0] = NewSprite(0, p1r.left + width/4-16, p1r.top + 160, SetupShipPuppet);
	p1sprites[0]->appPtr = (Ptr)0;
	p1sprites[1] = NewSprite(0, p1r.left + width/2-16, p1r.top + 160, SetupShipPuppet);
	p1sprites[1]->appPtr = (Ptr)1;
	p1sprites[2] = NewSprite(0, p1r.left + width*3/4-16, p1r.top + 160, SetupShipPuppet);
	p1sprites[2]->appPtr = (Ptr)2;
	p1sprites[3] = NewSprite(0,p1r.left + width/ 2-32,p1r.top+144,SetupHilight);
	p1hilight = 1;
	
	if (nPlayers == 2) {	
		p2sprites[0] = NewSprite(0, p2r.left + width/4-16, p2r.top + 160, SetupShipPuppet);
		p2sprites[0]->appPtr = (Ptr)0;
		p2sprites[1] = NewSprite(0, p2r.left + width/2-16, p2r.top + 160, SetupShipPuppet);
		p2sprites[1]->appPtr = (Ptr)1;
		p2sprites[2] = NewSprite(0, p2r.left + width*3/4-16, p2r.top + 160, SetupShipPuppet);
		p2sprites[2]->appPtr = (Ptr)2;
	    p2sprites[3] = NewSprite(0,p2r.left + width / 2-32,p2r.top+144,SetupHilight);
 		p2hilight = 1;
	}
	RGBFore(0,0,0);
	HideCursor();

	shipPic = GetPicture(rShipPic + p1hilight);
	SetRect(&p1sr,p1r.left+prectwidth/2 - (*shipPic)->picFrame.right/2,
				  p1r.top+50,p1r.left+prectwidth/2 + (*shipPic)->picFrame.right/2,
				  p1r.top+50+(*shipPic)->picFrame.bottom);
	DrawPicture(shipPic,&p1sr);

	SetRect(&p1mr,p1r.left+10,p1r.top+210,p1r.right - 10,p1r.bottom - 10);

	if (nPlayers == 2) {
		SetRect(&p2mr,p2r.left+10,p2r.top+210,p2r.right - 10,p2r.bottom - 10);
		SetRect(&p2sr,p2r.left+prectwidth/2 - (*shipPic)->picFrame.right/2,
				  p2r.top+50,p2r.left+prectwidth/2 + (*shipPic)->picFrame.right/2,
				  p2r.top+50+(*shipPic)->picFrame.bottom);
		DrawPicture(shipPic,&p2sr);
	}

	delay1 = TickCount();
	delay2 = TickCount();
	RunSAT(PrefFastDraw());
	PeekOffscreen();
	DrawPicture(shipPic,&p1sr);
	DrawShipMessage(p1hilight, &p1mr);

	if (nPlayers == 2) {
		DrawPicture(shipPic,&p2sr);	
		DrawShipMessage(p2hilight, &p2mr);
	}
	FadeUp(5);

	while (!p1sel || (nPlayers == 2 && !p2sel)) {
		l = TickCount();
		RunSAT(PrefFastDraw());
		
		GetKeys( (long *) km);
		if (IsPressed(escapeKey,km)) {
			*p1type = -1;
			for (i=0;i<4;i++) {
				p1sprites[i]->task = NULL;
				if (nPlayers==2) p2sprites[i]->task = NULL;
			}
			ShowCursor();
			return;
		}

		if (!p1sel && IsPressed(p1right,km) && TickCount()-delay1 > 5) {
			if (p1hilight < 2) {
				p1hilight ++;
				p1sprites[3]->position.h += width/4;	
				shipPic = GetPicture(rShipPic + p1hilight);
				DrawPicture(shipPic,&p1sr);
				delay1 = TickCount();
				DrawShipMessage(p1hilight, &p1mr);
			}
		}
		if (!p1sel && IsPressed(p1left,km) && TickCount()-delay1 > 5) {
			if (p1hilight > 0) {
				p1hilight--;
				p1sprites[3]->position.h -= width/4;	
				shipPic = GetPicture(rShipPic + p1hilight);
				DrawPicture(shipPic,&p1sr);
				delay1 = TickCount();
				DrawShipMessage(p1hilight, &p1mr);
			}
		}
		if (nPlayers == 2 && !p2sel && IsPressed(p2right,km) && TickCount() - delay2 > 5) {
			if (p2hilight < 2) {
				p2hilight ++;
				p2sprites[3]->position.h += width/4;	
				shipPic = GetPicture(rShipPic + p2hilight);
				DrawPicture(shipPic,&p2sr);
				delay2 = TickCount();
				DrawShipMessage(p2hilight, &p2mr);
			}
		}
		if (nPlayers == 2 && !p2sel && IsPressed(p2left,km) && TickCount() - delay2 > 5) {
			if (p2hilight > 0) {
				p2hilight --;
				p2sprites[3]->position.h -= width/4;	
				shipPic = GetPicture(rShipPic + p2hilight);
				DrawPicture(shipPic,&p2sr);
				delay2 = TickCount();
				DrawShipMessage(p2hilight, &p2mr);
			}
		}
		if (!p1sel && IsPressed(p1fire,km)) {
			p1sel = TRUE;
			if (p1hilight != 0) p1sprites[0]->task = NULL;
			if (p1hilight != 1) p1sprites[1]->task = NULL;
			if (p1hilight != 2) p1sprites[2]->task = NULL;
			p1sprites[3]->task = NULL;			
		}
		if (!p2sel && nPlayers == 2 && IsPressed(p2fire,km)) {
			p2sel = TRUE;
			if (p2hilight != 0) p2sprites[0]->task = NULL;
			if (p2hilight != 1) p2sprites[1]->task = NULL;
			if (p2hilight != 2) p2sprites[2]->task = NULL;
			p2sprites[3]->task = NULL;			
		}
		while (TickCount() - l < 2) ;
	}
	
	KillSprite(p1sprites[p1hilight]);
	if (nPlayers==2) KillSprite(p2sprites[p2hilight]);

	*p1type = p1hilight;
	if (nPlayers==2) *p2type = p2hilight;
	ShowCursor();
}

void DrawShipMessage(int ship, Rect *r) {
	Rect 		subr;
	Point 		p;
	Str31		name;
	Str255		buffer;
	
	subr = *r;
	subr.top += 12;
	RGBFore(0,0,0);
	PaintRect(r);

	GetIndString(name, rShipNameStrings, ship+1);
	GetIndString(buffer, rShipNameStrings, ship+4);

	RGBFore(65535,65535,0);

	MoveTo(r->left, r->top + 10);
	TextFace(bold);
	DrawString(name);
	TextFace(0);
	DrawStringInRect(buffer,&subr);

	RGBFore(65535,0,0);	
	MoveTo(r->left,r->top + 40);
	TextFace(bold);
	DrawString("\pStandard Equipment:");
	RGBFore(0,0,0);
	p.h = r->left + 3;
	p.v = r->top + 55;
	switch (ship) {
		case 0: // crab
			PlotOneItem(i8Shots,&p);p.h += 34;
			PlotOneItem(iShieldsTwo,&p);p.h += 34;			
			PlotOneItem(iEngineHalf,&p);p.h += 34;			
			break;
		case 1: // mantis
			PlotOneItem(i4Shots,&p);p.h += 34;
			PlotOneItem(iShieldsHalf,&p);p.h += 34;			
			PlotOneItem(iEngineTwo,&p);p.h += 34;			
			break;
		case 2: // moth
			PlotOneItem(i6Shots,&p);		p.h += 34;
			PlotOneItem(iShieldsOne,&p);	p.h += 34;
			PlotOneItem(iEngineOne,&p);	p.h += 34;
			break;
	}
}