/* Cheat.c: Code to handle Cheat Mode screen */
#include "MaiNResources.h"
#include "SAT.h"
#include "BuyableITems.h"
#include "PlayerShip.h"
#include "Sprites.h"

#define rCheatPict 159
#define rHaloCICNid 1599

PicHandle	gCheatPicH = NULL;
Boolean		gSelectedItems[2][28] ={
				{0,0,0,0,1,  0,0,0,0,0,  0,0,0,1,0, 0,0,1,0,0, 0,0,0,0,0,  0,0,0},
				{0,0,0,0,1,  0,0,0,0,0,  0,0,0,1,0, 0,0,1,0,0, 0,0,0,0,0,  0,0,0}};
int			gShipType[2];
int			gSelectedPlayerNum=0, gCheatLevel=1;
Rect		gItemRects[28], gShipTypeRects[3], gPlayerRects[2], gDoneButtonRect, gStartGameRects[2], gLevelChangeRects[2], gLevelInfoRect;
CIconHandle	gHaloIcon;
Boolean	 	gInCheat = FALSE;
extern Rect gGameRect;

void DrawCyanString(StringPtr str);
void PlotHalo(Point p);
int	DrawEquipmentSequence(StringPtr str, int h, int v, int specialNumber, int index, int endingNumber);
void DrawShipTypeSequence(int h, int v);
void RGBFore(unsigned int red,unsigned int green,unsigned int blue);
void PlotOneShip(int which, Point *p);
void SetSelection(int toSet, int starting, int index, int ending, Boolean toggleIfSet);
void ToggleSelection(int toSet);
Boolean HandleCheatModeClick(Point where);
void PlayCheatGame(int nplayers);
void DrawPictureAt(PicHandle pic, Point p);
int GetCheatLevel(void);

void DrawCheat(void) {
	PicHandle 	hPic;
	Rect 		r;
	Point 		p;
	short		num;
	int			i;
	Str31		buffer;
	
	gInCheat = TRUE;
	if (!gCheatPicH) {
		gCheatPicH = GetPicture(rCheatPict);
		// preset selected items:
		gShipType[0] = 2;
		gShipType[1] = 2;
		gHaloIcon = GetCIcon(rHaloCICNid);
	}
	
	hPic = GetPicture(rSmallLogo);
	r = (*hPic)->picFrame;
	OffsetRect(&r,10,20);
	DrawPicture(hPic,&r);

	r = (*gCheatPicH)->picFrame;
	OffsetRect(&r, 10, 20 + (*hPic)->picFrame.bottom + 10);
	DrawPicture(gCheatPicH, &r);

	GetFNum("\pGeneva", &num);
	TextFont(num);TextSize(9);

	p.v = r.bottom + 20;
	MoveTo(10, p.v);

	RGBFore(0,65535,0);TextFace(bold);DrawString("\pSetup for Player: ");TextFace(0);
	MoveTo(150, p.v); DrawCyanString("\pOne");
	MoveTo(200, p.v); DrawCyanString("\pTwo");
	SetRect(&gPlayerRects[0], 142, p.v-20, 142+48, p.v+28);
	SetRect(&gPlayerRects[1], 192, p.v-20, 192+48, p.v+28);

	p.h = gSelectedPlayerNum * 50 + 142;	
	p.v -=  20;
	PlotHalo(p);
	p.h = 10;p.v+=50;

	MoveTo(10, p.v);
	DrawCyanString("\pShip Type:");
	DrawShipTypeSequence(10, p.v);
	DrawEquipmentSequence("\pShield Strength:", 310, p.v, iShieldsHalf, iShieldsOne, iShieldsFour);

	p.v += 60;
	DrawEquipmentSequence("\pNumber of Shots:", 10, p.v, i4Shots, i6Shots, i12Shots);
	DrawEquipmentSequence("\pEngine Strength:", 310, p.v, iEngineHalf, iEngineOne, iRetroEngine);

	p.v += 60;
	DrawEquipmentSequence("\pShot Bonuses:", 10, p.v, iFastShots, iAutoShots, iScatterShots);
	DrawEquipmentSequence("\pSpecial Equipment:", 310, p.v, -99, iStarburst, iRecharge);

	p.v += 70;
	MoveTo(10, p.v);
	DrawCyanString("\pStarting Level: ");
	GetPen(&p);
	SetRect(&gLevelInfoRect, p.h, p.v-12, 0, p.v+3);

	NumToString(gCheatLevel, buffer);	
	DrawCyanString(buffer);
	GetPen(&p);
	gLevelInfoRect.right = p.h + 8;
	
	p.h =100; p.v -= 15;
	DrawPictureAt(GetPicture(rArrowUpPict), p);
	SetRect(&gLevelChangeRects[0], p.h, p.v, p.h + 16, p.v +16);
	p.v += 20;
	DrawPictureAt(GetPicture(rArrowDownPict), p);
	SetRect(&gLevelChangeRects[1], p.h, p.v, p.h + 16, p.v +16);

	p.h = gGameRect.right/2+10;
	p.v = gGameRect.top + 48;
	DrawButtonAndLabel(GetPicture(rRedOutPict), GetPicture(rBeginOnePict), p);
	SetRect(&gStartGameRects[0], p.h, p.v, p.h+30, p.v+30);

	p.v += 38;
	DrawButtonAndLabel(GetPicture(rRedOutPict), GetPicture(rBeginTwoPict), p);
	SetRect(&gStartGameRects[1], p.h, p.v, p.h+30, p.v+30);

	hPic = GetPicture(rDoneOutPict);
	r = (*hPic)->picFrame;
	OffsetRect(&r, gGameRect.right - r.right - 10, gGameRect.bottom - r.bottom - 10);
	DrawPicture(hPic, &r);
	gDoneButtonRect = r;
}

void SetSelection(int toSet, int starting, int index, int ending, Boolean toggleIfSet) {
	Boolean wasSet = gSelectedItems[gSelectedPlayerNum][toSet];

	if (starting!= -99) {
		if (gSelectedItems[gSelectedPlayerNum][starting+3]) {
			InvalRect(&gItemRects[starting+3]);
			gSelectedItems[gSelectedPlayerNum][starting+3] = 0;
		}
	}
	while (index <= ending) {
		if (gSelectedItems[gSelectedPlayerNum][index+3]) {
			InvalRect(&gItemRects[index+3]);
			gSelectedItems[gSelectedPlayerNum][index+3] = 0;	
		}
		index++;
	}
	if (wasSet && toggleIfSet) gSelectedItems[gSelectedPlayerNum][toSet] = 0;
	else gSelectedItems[gSelectedPlayerNum][toSet] = 1;
	InvalRect(&gItemRects[toSet]);
}


void ToggleSelection(int toSet) {
	gSelectedItems[gSelectedPlayerNum][toSet] = !gSelectedItems[gSelectedPlayerNum][toSet];
	InvalRect(&gItemRects[toSet]);
}

Boolean HandleCheatModeClick(Point where) { // returns DONE
	int i;
	
	if (PtInRect(where, &gDoneButtonRect)) {
		if (TrackMouseInButton(GetPicture(rDoneOutPict), 
				GetPicture(rDoneInPict), gDoneButtonRect)) {
					gInCheat = FALSE;
					return TRUE;
				}
	}
	for (i=0; i<2;i++) 
		if (PtInRect(where, &gStartGameRects[i])) {
			if (TrackMouseInButton(GetPicture(rRedOutPict), 
				GetPicture(rRedInPict), gStartGameRects[i])) {
					PlayCheatGame(i+1);
					return FALSE;
				}
			}

	for (i=0;i<2;i++) 
		if (PtInRect(where, &gLevelChangeRects[i])) {
			gCheatLevel += (i==0)?1:-1;
			if (gCheatLevel < 1) gCheatLevel=1;
			if (gCheatLevel > 25) gCheatLevel=25;
			InvalRect(&gLevelInfoRect);
			return FALSE;
		}
			
	for (i=0;i<2;i++) 
		if (PtInRect(where, &gPlayerRects[i])) {
			gSelectedPlayerNum = 1-gSelectedPlayerNum;
			InvalRect(&gGameRect);
			return FALSE;
		}
	for (i=0;i<3;i++) 
		if (PtInRect(where, &gShipTypeRects[i])) {
			InvalRect(&gShipTypeRects[gShipType[gSelectedPlayerNum]]);
			gShipType[gSelectedPlayerNum] = i;
			InvalRect(&gShipTypeRects[gShipType[gSelectedPlayerNum]]);
			return FALSE;
		}
	for (i=0;i<28;i++) 
		if (PtInRect(where, &gItemRects[i])) {
			switch (i-3) {
				case  i4Shots:	
				case  i6Shots:
				case  i8Shots:
				case  i12Shots: SetSelection(i, i4Shots, i6Shots, i12Shots, FALSE);break;
				case  iShieldsHalf:
				case  iShieldsOne:
				case  iShieldsTwo:
				case  iShieldsThree:
				case  iShieldsFour:SetSelection(i, iShieldsHalf, iShieldsOne, iShieldsFour, FALSE);break;
				case  iEngineHalf:
				case  iEngineOne:
				case  iEngineTwo:
				case  iEngineThree:
				case  iEngineFour: SetSelection(i, iEngineHalf, iEngineOne, iEngineFour, FALSE);break;
				case  iDoubleBeamShots:
				case  iTripleBeamShots:SetSelection(i, -99, iDoubleBeamShots, iTripleBeamShots, TRUE);break;
				case  iRetroEngine:
				case  iFastShots:
				case  iAutoShots:
				case  iSafeShots:
				case  iScatterShots:
				case  iStarburst:
				case  iSuperShields:
				case  iGrabber:
				case  iMine:
				case  iHyperspace:
				case  iRecharge: ToggleSelection(i);break;
			}
		}
	return FALSE;
}


void DrawShipTypeSequence(int h, int v) {
	Point p;
	int i;

	p.h = h+24;
	p.v = v+6;
	for (i=0;i<3;i++) {
		PlotOneShip(i, &p);
		if (gShipType[gSelectedPlayerNum]==i) PlotHalo(p);
		SetRect(&gShipTypeRects[i], p.h-8,p.v-8,p.h+40,p.v+40);
		p.h += 48; 	
	}
}


int	DrawEquipmentSequence(StringPtr str, int h, int v, int specialNumber, int index, int endingNumber) {
	Point p;
	Rect r;

	MoveTo(h,v);
	DrawCyanString(str);
	p.h = h+24;
	p.v = v+6;
	if (specialNumber != -99) {
		PlotOneItem(specialNumber, &p);
		if (gSelectedItems[gSelectedPlayerNum][specialNumber+3]) PlotHalo(p);
		SetRect(&gItemRects[specialNumber+3], p.h-8,p.v-8,p.h+40,p.v+40);
		p.h += 48; 
	}
	while (index <= endingNumber) {
		PlotOneItem(index, &p);
		if (gSelectedItems[gSelectedPlayerNum][index+3]) PlotHalo(p);
		SetRect(&gItemRects[index+3], p.h-8,p.v-8,p.h+40,p.v+40);
		p.h += 48; 	
		index++;
	}
	return p.h;
}

void PlotHalo(Point p) {
	Rect r;
	SetRect(&r, p.h - 8, p.v - 8, p.h + 40, p.v +40);
	PlotCIcon (&r, gHaloIcon);
}

void DrawCyanString(StringPtr str) {
	RGBFore(32767,65535,65535);
	DrawString(str);
	RGBFore(0,0,0);
}

/*
#define iEngineHalf		-3                1
#define iShieldsHalf	-2                2
#define i4Shots		-1               	  3
#define iFastShots 	0              		  4
#define i6Shots		1              		  5

#define i8Shots		2              		  6
#define i12Shots	3              		  7
#define iAutoShots	4              		  8
#define iSafeShots	5              		  9
#define iDoubleBeamShots 6                10

#define iTripleBeamShots 7                11
#define iScatterShots	8                12
#define iShieldRecharge 9                13
#define iShieldsOne		10                14
#define iShieldsTwo		11                15

#define iShieldsThree	12                16
#define iShieldsFour	13                17
#define iEngineOne		14                18
#define iEngineTwo		15                19
#define iEngineThree 	16                20

#define iEngineFour 	17                21
#define iRetroEngine  	18                22
#define iStarburst		19                23

#define iSuperShields	20                24
#define iGrabber		21                25
#define iMine			22                26
#define iHyperspace 	23                27
#define iRecharge		24                28*/


void SetUpCheatShip(PlayerShipData *data, int playerNum) {
	int i;
	data->graphicID = gShipType[playerNum-1];

	for (i=0;i<28;i++) {
		if (gSelectedItems[playerNum-1][i]) {
			switch (i-3) {
				case  i4Shots:data->maxNumberShots = 4;break;
				case  i6Shots:data->maxNumberShots = 6;break;
				case  i8Shots:data->maxNumberShots = 8;break;
				case  i12Shots: data->maxNumberShots = 12;break;
				case  iShieldsHalf:data->shieldMax = data->shields = 4000;break;
				case  iShieldsOne:data->shieldMax = data->shields = 6000;break;
				case  iShieldsTwo:data->shieldMax = data->shields = 8000;break;
				case  iShieldsThree:data->shieldMax = data->shields = 12000;break;
				case  iShieldsFour:data->shieldMax = data->shields = 16000;break;
				case  iEngineHalf:data->thrustRate=moveGranularity/10;data->topSpeed = 4*moveGranularity;break;
				case  iEngineOne:data->thrustRate=moveGranularity/8;data->topSpeed = 6*moveGranularity;break;
				case  iEngineTwo:data->thrustRate=moveGranularity/5;data->topSpeed = 9*moveGranularity;break;
				case  iEngineThree:data->thrustRate = moveGranularity/3;data->topSpeed =11*moveGranularity;break;
				case  iEngineFour: data->thrustRate =moveGranularity/2;data->topSpeed = 13*moveGranularity;break;
				case  iDoubleBeamShots:data->shotsInOneShot = 2;break;
				case  iTripleBeamShots:data->shotsInOneShot = 3;break;
				case  iRetroEngine:data->retroRate = moveGranularity/8;break;
				case  iFastShots:data->shotSpeed = 6;break;
				case  iAutoShots:data->shotsPerBurst = 16;
									data->shotDelay = 3;break;
				case  iSafeShots:data->safeShots= TRUE;break;
				case  iScatterShots:data->scatterShots =  TRUE;break;

				case  iStarburst:	data->specialSelected = data->nSpecials;
									data->specials[data->nSpecials++] = sStarburst;
									break;	
				case  iSuperShields:data->specialSelected = data->nSpecials;
									data->specials[data->nSpecials++] = sSuperShields;
									break;	
				case  iGrabber:data->specialSelected = data->nSpecials;
									data->specials[data->nSpecials++] = sGrabber;
									break;	
				case  iMine:data->specialSelected = data->nSpecials;
									data->specials[data->nSpecials++] = sMine;
									break;	
				case  iHyperspace:data->specialSelected = data->nSpecials;
									data->specials[data->nSpecials++] = sHyperspace;
									break;	
				case  iRecharge: data->specialSelected = data->nSpecials;
									data->specials[data->nSpecials++] = sRecharge;
									break;	
			}
		}
	}
}


Boolean InCheatMode(void) {
	return gInCheat;
}

int GetCheatLevel(void) {
	return gCheatLevel;
}