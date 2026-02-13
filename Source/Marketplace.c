#include <math.h>
#include "SAT.h"
#include "AsteraxResources.h"
#include "Sprites.h"
#include "Game.fp"
#include "gamma.h"
#include "Pause.h"
#include "marketplace.h"
#include "Strings.h"

extern RGBColor cBlack;
extern int gLevel,gNPlayers;
extern SpritePtr p1ship, p2ship;
extern Rect p1scoreR,p2scoreR, p1ShieldR, p2ShieldR;
extern Rect p1InfoRect,p2InfoRect;
extern Boolean levelOver, noMoreRocks, gameOver;
extern long p1ShieldRWidth,p2ShieldRWidth;
extern int shieldWordLength;
extern int p1Lives, p2Lives;
extern int p1Bank, p2Bank;

void FlyShipToCenter(SpritePtr spr, PlayerShipData *ship);
void FlyShipToSprite(SpritePtr spr, PlayerShipData *ship, SpritePtr target);
void HilightClosest(SpritePtr spr, int *closest, int *oldclosest);
Boolean CheckAnimationKeyBreaks();


#define MaxDisplayItems	nMaxItems * 2 + 10

SpritePtr stores[nNumStores];
SpritePtr items[nMaxItems], hilightSprite;
SpritePtr displayItems[MaxDisplayItems];
int		  exitSprite,curNumItems,nDisplayItems;
int		  storeAngles[nNumStores],itemAngles[nMaxItems];

RGBColor cLineColor;

void HandlePurchases() {
	Str255 buffer, pnumBuffer, marketStr;
	long t;
	int done,i,entered;
	PlayerShipData *p1dat = NULL, *p2dat = NULL;

	if (p1Lives>0) p1dat = (PlayerShipData *)p1ship->appPtr;
	if (p2Lives>0) p2dat = (PlayerShipData *)p2ship->appPtr;

	if (p1dat) {
		p1dat->noShieldLoss = TRUE;
		RedrawPlayerShipInfo(p1dat);
	}
	if (p2dat) {
		p2dat->noShieldLoss = TRUE;
		RedrawPlayerShipInfo(p2dat);
	}

	GetIndString(marketStr, rGeneralStrings, rEnteringMarket);
	SetupMarketplaceSprites();
	if (p1Lives > 0) {
		if (p2Lives> 0) {
			GetIndString(pnumBuffer, rGeneralStrings, rPlayerOne);
			sprintf((char *)buffer,"%#s %#s",pnumBuffer, marketStr);
			CtoPstr((char *)buffer);
			DisplayStatusMessage(buffer);
		}
		p1dat->suspend = FALSE;
		if (p2Lives) {
			p2dat->suspend= TRUE;
			p2ship->position.h = -80;
			p2ship->position.v = -80;
			p2ship->speed.h = 0;
			p2ship->speed.v = 0;
		}
		HandleOnePlayer(p1ship,p1dat);
		p1ship->position.h  = -2000;
		p1ship->position.v  = -2000;
		
		p2dat->suspend= FALSE;	
	}
	if (!gameOver && p2Lives > 0) {
		PaintRect(&gGameRect);
		if (p1Lives >0) {
			GetIndString(pnumBuffer, rGeneralStrings, rPlayerTwo);
			sprintf((char *)buffer,"%#s %#s",pnumBuffer, marketStr);
			CtoPstr((char *)buffer);
			DisplayStatusMessage(buffer);
		}
		p2dat->suspend = FALSE;
		if (p1Lives) {
			p1dat->suspend= TRUE;
			p1ship->position.h = gGameRect.left;
			p1ship->position.v = gGameRect.bottom;
			p1ship->speed.h = 0;
			p1ship->speed.v = 0;
		}
		HandleOnePlayer(p2ship,p2dat);
		p1dat->suspend= FALSE;	

	}

	GetRidOfStores();
	if (p1dat) {
		p1Bank = p1dat->money;
		p1dat->money = 0;
		p1dat->noShieldLoss = FALSE;
	}
	if (p2dat) {
		p2Bank = p2dat->money;
		p2dat->money = 0;
		p2dat->noShieldLoss = FALSE;
	}
}
	
void HandleOnePlayer(SpritePtr spr, PlayerShipData *ship) {
	int done,i,j,entered,oldclosest,closest,deltaclose;
	long l;
	float costheta,sintheta;
	
	spr->position.h = gGameRect.right/2;
	spr->position.v = gGameRect.bottom/2;
	spr->speed.h = 0;
	spr->speed.v = 0;

	spr->speed.h = spr->speed.v = 0;

	done = FALSE;
	entered = FALSE;


	FadeDown(4);
	DrawMarketplaceBackground();
	PeekOffscreen();
	RedrawPlayerShipInfo(ship);
	DrawPlayerShields(ship);
	DrawScore(ship);
	DrawStructure();

	oldclosest = -1;
	RunSAT(PrefFastDraw());
	FadeUp(4);
	cLineColor.red = 65535; cLineColor.green = 65535; cLineColor.blue = 0;
	do {
		l = TickCount();
		RunSAT(PrefFastDraw()); /* Run the animation */

// Check for break conditions:
		if (CheckAnimationKeyBreaks()) break;
		if (ship->redrawInfo) RedrawPlayerShipInfo(ship);
		if (ship->redrawSpecial) RedrawPlayerShipSpecial(ship);

		for (i=0;i<nNumStores;i++) {
			if (stores[i]->appLong == 1) {
				nDisplayItems = 0;
				switch (stores[i]->mode) {								
					case weaponStore:FadeDown(4);
									 HideStoreSprites();
									 SetupWeaponStoreSprites(ship);
									 RunStoreAnimation(weaponStore, spr, ship);
									 DisposeWeaponStoreSprites();
									 break;				
					case shieldStore:FadeDown(4);
									 HideStoreSprites();
									 SetupShieldStoreSprites(ship);
									 RunStoreAnimation(shieldStore, spr, ship);
									 DisposeShieldStoreSprites();
									 break;				
					case engineStore:FadeDown(4);
									 HideStoreSprites();
									 SetupEngineStoreSprites(ship);
									 RunStoreAnimation(engineStore, spr, ship);
									 DisposeEngineStoreSprites();
									 break;			
					case stuffStore: FadeDown(4);
									 HideStoreSprites();
									 SetupStuffStoreSprites(ship);
									 RunStoreAnimation(stuffStore, spr, ship);
									 DisposeStuffStoreSprites();
									 break;						 									 
					case financeStore:
									 SetInsuranceCost(CalculateShipPremium(ship));
									 FadeDown(4);
									 HideStoreSprites();
									 SetupFinanceStoreSprites(ship);
									 RunStoreAnimation(financeStore, spr, ship);
									 DisposeFinanceStoreSprites();
									 break;						 									 
					case exitStore:done = TRUE;	break;
				}
				if (stores[i]->mode != exitStore) {
					DrawMarketplaceBackground();
					ShowStoreSprites();
					RedrawPlayerShipInfo(ship);
					DrawPlayerShields(ship);
					DrawScore(ship);
					DrawStructure();														
					FadeUp(4);
				
				}
				stores[i]->appLong = 0;
			}
		}
		while (l > TickCount() - GetPrefSpeed()) /*Maximize speed to 30 fps*/;
	} while (!done);
}

void RunStoreAnimation(int whichStore, SpritePtr spr, PlayerShipData *ship) {
	int 				done,i,closest,oldclosest,deltaclose,angle;
	long 				l;
	Point 				oldPos = spr->position,p;
	float 				costheta,sintheta;
	PlayerShipData		*ins;


	done = FALSE;
	oldclosest = -2;
	spr->speed.h = 0;spr->speed.v = 0;
	spr->position.h = gGameRect.right/2 + gGameRect.left;
	spr->position.v = gGameRect.bottom/2 + gGameRect.top;
	PeekOffscreen();
	RedrawPlayerShipInfo(ship);
	DrawPlayerShields(ship);
	DrawScore(ship);
	DrawStructure();
	
	oldclosest=-2;
	HilightClosest(spr,&closest,&oldclosest);
	RunSAT(PrefFastDraw());
	FadeUp(4);
	oldclosest=-2;
	HilightClosest(spr,&closest,&oldclosest);

	do {
		l = TickCount();
		RunSAT(PrefFastDraw()); /* Run the animation */
		if (CheckAnimationKeyBreaks()) break;
		if (ship->redrawInfo) RedrawPlayerShipInfo(ship);
		if (ship->redrawSpecial) RedrawPlayerShipSpecial(ship);

		HilightClosest(spr,&closest,&oldclosest);

		if (spr->speed.h || spr->speed.v) {
			if (closest == -1) {
				done = TRUE;
				FlyShipToSprite(spr,ship,stores[exitSprite]);
				spr->speed.h = spr->speed.v=  0;
			} else {
				// buy it
				FlyShipToSprite(spr,ship,items[closest]);
				spr->speed.h = spr->speed.v = 0;
				DrawStructure();														
				RedrawPlayerShipInfo(ship);
				DrawPlayerShields(ship);
				DrawScore(ship);
				
				RemoveDisplayItems();
				p.h = gGameRect.left + 8;
				p.v = gGameRect.top + 50;
				ins = GetPlayerInsurance(ship);
				switch (whichStore) {
					case weaponStore:DrawWeaponStoreCurrentEquipment(ship,&p, NULL);break;
					case shieldStore:DrawShieldStoreCurrentEquipment(ship,&p, NULL);break;
					case engineStore:DrawEngineStoreCurrentEquipment(ship,&p, NULL);break;
					case stuffStore:DrawStuffStoreCurrentEquipment(ship,&p, NULL);break;
					case financeStore:
									DrawWeaponStoreCurrentEquipment(ship,&p, ins);
									DrawShieldStoreCurrentEquipment(ship,&p, ins);
									DrawEngineStoreCurrentEquipment(ship,&p, ins);
									DrawStuffStoreCurrentEquipment(ship,&p, ins);
									break;
				}				
				FlyShipToCenter(spr,ship);		
				items[closest]->appLong = 0;
				spr->speed.h = spr->speed.v = 0;
			}
		}
		while (l > TickCount() - GetPrefSpeed()) /*Maximize speed to 30 fps*/;

	} while (!done);

	spr->position.h = gGameRect.right/2 + gGameRect.left;
	spr->position.v = gGameRect.bottom/2 + gGameRect.top;
	spr->mode = 0;
	FadeDown(4);
	PeekOffscreen();
	RedrawPlayerShipInfo(ship);
	DrawPlayerShields(ship);
	DrawScore(ship);
	DrawStructure();
		
	RunSAT(PrefFastDraw());
}

void FlyShipToSprite(SpritePtr spr, PlayerShipData *ship, SpritePtr target) {
	long l;
	ship->target = target->position;
	ship->noControl = TRUE;
	do {
		l = TickCount();
		RunSAT(PrefFastDraw()); /* Run the animation */

		if ((ABS(spr->position.h - target->position.h) < 6) &&
			(ABS(spr->position.v - target->position.v) < 6)) break;
		while (l > TickCount() - GetPrefSpeed()) /*Maximize speed to 30 fps*/;	
	} while (!Button());
	ship->noControl = FALSE;
}

void FlyShipToCenter(SpritePtr spr, PlayerShipData *ship) {
	long l;
	ship->target.h = gGameRect.right/2 + gGameRect.left;
	ship->target.v = gGameRect.bottom/2 + gGameRect.top;
	ship->noControl = TRUE;
	do {
		l = TickCount();
		RunSAT(PrefFastDraw()); /* Run the animation */

		if ((ABS(spr->position.h - ship->target.h) < 6) &&
			(ABS(spr->position.v - ship->target.v) < 6)) break;
		while (l > TickCount() - GetPrefSpeed()) /*Maximize speed to 30 fps*/;	
	} while (!Button());
	ship->noControl = FALSE;
}

void HilightClosest(SpritePtr spr, int *closest, int *oldclosest) {
	int 		deltaclose,i;
	Rect 		r;
	RGBColor 	c;
	Point 		w;
	GrafPtr		oldPort;
	Boolean 	redraw=FALSE;

	deltaclose = abs(spr->mode - storeAngles[exitSprite]);

	for (*closest=-1,i=0;i<curNumItems;i++) {
		if (abs(spr->mode-itemAngles[i]) < deltaclose) {
			deltaclose = abs(spr->mode-itemAngles[i]);
			*closest = i;
		}
	}

	if (*oldclosest != *closest) redraw=TRUE;
	if (redraw) {
		SetRect(&r,gGameRect.right - 190,gGameRect.top + 40,
				   gGameRect.right - 20,gGameRect.top + 90);
		c.red=c.green=c.blue=0;RGBForeColor(&c);
		PaintRect(&r);
	}
	*oldclosest = *closest;
	if (*closest == -1) {
		hilightSprite->position.h = stores[exitSprite]->position.h;
		hilightSprite->position.v = stores[exitSprite]->position.v+8;
	} else {
		 if (redraw) {
		 	InsetRect(&r,4,4);
		 	GetPort(&oldPort);
		 	 SetPort(gSAT.backScreen);
		 	 RGBFore(0,0,0);
		 	 PaintRect(&r);
		 	 c.green=c.blue=65535;RGBForeColor(&c);
			 TextFace(0);
 			 DisplayItemMessage(items[*closest],&r);
			 TextFace(bold);
			 SetPort(oldPort);
			 SATBackChanged(&r);
 		}
		 hilightSprite->position = items[*closest]->position;
		 hilightSprite->position.h-=16;
		 hilightSprite->position.v-=16;
	}
}

void SetupStoreSprites(int start, int end) {
	SpritePtr new;
	int i,k=21,range;
	int midh = gGameRect.right/2 + gGameRect.left;
	int midv = gGameRect.bottom/2 + gGameRect.top;
	float sinth, costh;
	GrafPtr oldPort;
	RGBColor c;
	Rect r;

	GetPort(&oldPort);
	SetPort(gSAT.backScreen);
	SetRect(&r,0,0,gSAT.offSizeH,gSAT.offSizeV);
	c.red= c.green = c.blue = 0;
	RGBForeColor(&c);
	PaintRect(&r);


	c.green = 32767;c.blue =32767;
	RGBForeColor(&c);
	PenSize(3,3);
	FrameRoundRect(&r,16,16);
	PenSize(1,1);
	c.blue =0;
	RGBForeColor(&c);

	TextFont(geneva);
	TextFace(bold);
	TextSize(9);
	if (midh > midv) midh= midv;
	if (midv > midh) midv = midh;
	range = end-start;
	for (i=0;i<range;i++) {
		sinth = sin((i+1)*6.28/(range+1)+3.14/2);
		costh = cos((i+1)*6.28/(range+1)+3.14/2);
		itemAngles[i] = (i*36/(range+1)+k)%36;
		items[i] = NewSprite(-1,gGameRect.right/2 + costh * midh * .6 -16,
								midv + sinth * midv * .6-16,SetupItem);
		items[i]->mode = i+start;	
		MoveTo(items[i]->position.h + 32,items[i]->position.v - 2);
		DrawInt(GetItemCost(i+start));
	}
	curNumItems = range;
	hilightSprite = NewSprite(0,items[0]->position.h-16,items[0]->position.v-16,
									SetupHilight);
									
									
	RGBForeColor(&cBlack);									
	DrawStars();	
	SetPort(oldPort);
	SATBackChanged(&r);
	RGBForeColor(&cBlack);								
}

void CreateNewDisplayItem(Point *p, int mode, Boolean insured) {
	if (nDisplayItems == MaxDisplayItems) return;
	displayItems[nDisplayItems] = NewSprite(0,p->h,p->v,SetupItem);
	displayItems[nDisplayItems]->mode = mode;
	nDisplayItems++;

	if (insured) {
		displayItems[nDisplayItems]= NewSprite(0, p->h + 32, p->v+8, SetupInsuranceItem);
		nDisplayItems++;
	}
	p->v += 34;
}

void DrawCurrentEquipmentMessage(Point *p) {
	GrafPtr		oldPort;
	Rect		r;
	Str31		buffer;
	
	GetPort(&oldPort);
	SetPort(gSAT.backScreen);
	MoveTo(p->h,p->v);
	RGBFore(65535,0,65535);
	TextFace(bold);

	GetIndString(buffer, rGeneralStrings, rCurrentEquipment);
	DrawString(buffer);

	SetPort(oldPort);
	SetRect(&r,p->h,p->v-15,p->h+100,p->v+3);
	SATBackChanged(&r);
	p->v += 10;

}

int GetShotItemNum(int shots) {
	switch (shots) {
		case 6:return i6Shots;
		case 8:return i8Shots;
		case 12:return i12Shots;
	}
	return i4Shots;
}


void DrawWeaponStoreCurrentEquipment(PlayerShipData *data, Point *p, PlayerShipData *ins) {
	CreateNewDisplayItem(p,GetShotItemNum(data->maxNumberShots),(ins && ins->maxNumberShots == data->maxNumberShots));
	if (data->shotSpeed == 6) CreateNewDisplayItem(p,iFastShots, (ins && ins->shotSpeed == 6));
	if (data->safeShots) CreateNewDisplayItem(p,iSafeShots, (ins && ins->safeShots));
	if (data->shotsPerBurst==16) CreateNewDisplayItem(p,iAutoShots, (ins && ins->shotsPerBurst == 16));
	if (data->shotsInOneShot==2) CreateNewDisplayItem(p,iDoubleBeamShots, (ins && ins->shotsInOneShot == 2));
	if (data->shotsInOneShot==3) CreateNewDisplayItem(p,iTripleBeamShots, (ins && ins->shotsInOneShot == 3));
	if (data->scatterShots) CreateNewDisplayItem(p,iScatterShots, (ins && ins->scatterShots));
}


void SetupWeaponStoreSprites(PlayerShipData *data) {
	Point p;
	
	SetupStoreSprites(0, nWeapons);
	p.h = gGameRect.left + 8;
	p.v = gGameRect.top + 40;
	DrawCurrentEquipmentMessage(&p);
	DrawWeaponStoreCurrentEquipment(data,&p, NULL);
}

void DrawShieldStoreCurrentEquipment(PlayerShipData *data,	Point *p, PlayerShipData *ins) {
	if (data->shieldMax == 6000) CreateNewDisplayItem(p,iShieldsOne,(ins && ins->shieldMax == 6000));
	if (data->shieldMax == 8000) CreateNewDisplayItem(p,iShieldsTwo,(ins && ins->shieldMax == 8000));
	if (data->shieldMax == 12000) CreateNewDisplayItem(p,iShieldsThree, (ins && ins->shieldMax == 12000));
	if (data->shieldMax == 16000) CreateNewDisplayItem(p,iShieldsFour, (ins && ins->shieldMax == 16000));
}

void SetupShieldStoreSprites(PlayerShipData *data) {
	Point p;
	SetupStoreSprites(nWeapons,nShields+nWeapons);
	p.h = gGameRect.left + 8;
	p.v = gGameRect.top + 40;
	DrawCurrentEquipmentMessage(&p);
	DrawShieldStoreCurrentEquipment(data,&p, NULL);
}

void DrawEngineStoreCurrentEquipment(PlayerShipData *data,Point *p, PlayerShipData *ins) {
	if (data->thrustRate == moveGranularity /8) CreateNewDisplayItem(p,iEngineOne, (ins && ins->thrustRate == moveGranularity /8));
	if (data->thrustRate == moveGranularity /5) CreateNewDisplayItem(p,iEngineTwo, (ins && ins->thrustRate == moveGranularity /5));
	if (data->thrustRate == moveGranularity /3) CreateNewDisplayItem(p,iEngineThree, (ins && ins->thrustRate == moveGranularity /3));
	if (data->thrustRate == moveGranularity /2) CreateNewDisplayItem(p,iEngineFour, (ins && ins->thrustRate == moveGranularity /2));
	if (data->retroRate) CreateNewDisplayItem(p,iRetroEngine, (ins && ins->retroRate));
}

void SetupEngineStoreSprites(PlayerShipData *data) {
	Point p;
	p.h = gGameRect.left + 8;
	p.v = gGameRect.top + 40;
	SetupStoreSprites(nShields+nWeapons,nShields+nWeapons+nEngines);
	DrawCurrentEquipmentMessage(&p);
	DrawEngineStoreCurrentEquipment(data,&p, NULL);
}

void DrawStuffStoreCurrentEquipment(PlayerShipData *data, Point *p, PlayerShipData *ins) {
	int 		i;
	int			insurCount[nStuff];
	
	if (ins) {
		for (i=0;i<nStuff;i++) insurCount[i] = 0;
		for (i=0;i<ins->nSpecials;i++) {
			insurCount[ins->specials[i]]++;
		}
	}
	
	for (i=0;i<data->nSpecials;i++) {
		switch (data->specials[i]) {
			case sStarburst:	CreateNewDisplayItem(p,iStarburst, (ins && insurCount[data->specials[i]]));break;
			case sSuperShields:	CreateNewDisplayItem(p,iSuperShields, (ins && insurCount[data->specials[i]]));break;
			case sGrabber:		CreateNewDisplayItem(p,iGrabber, (ins && insurCount[data->specials[i]]));break;
			case sMine:			CreateNewDisplayItem(p,iMine, (ins && insurCount[data->specials[i]]));break;
			case sHyperspace:	CreateNewDisplayItem(p,iHyperspace, (ins && insurCount[data->specials[i]]));break;
			case sRecharge:		CreateNewDisplayItem(p,iRecharge, (ins && insurCount[data->specials[i]]));break;	
		}
		if (ins && insurCount[data->specials[i]]) insurCount[data->specials[i]]--;
	}
}

void SetupStuffStoreSprites(PlayerShipData *data) {
	Point p;
	p.h = gGameRect.left + 8;
	p.v = gGameRect.top + 40;

	SetupStoreSprites(nShields+nWeapons+nEngines,nShields+nWeapons+nEngines+nStuff);
	DrawCurrentEquipmentMessage(&p);
	DrawStuffStoreCurrentEquipment(data,&p, NULL);
}

void SetupFinanceStoreSprites(PlayerShipData *data) {
	Point p;
	p.h = gGameRect.left + 8;
	p.v = gGameRect.top + 40;

	SetupStoreSprites(nShields+nWeapons+nEngines+nStuff,
					  nShields+nWeapons+nEngines+nStuff+nFinance);
	DrawCurrentEquipmentMessage(&p);

	DrawWeaponStoreCurrentEquipment(data,&p, GetPlayerInsurance(data));
	DrawShieldStoreCurrentEquipment(data,&p, GetPlayerInsurance(data));
	DrawEngineStoreCurrentEquipment(data,&p, GetPlayerInsurance(data));
	DrawStuffStoreCurrentEquipment(data,&p, GetPlayerInsurance(data));
}

void RemoveDisplayItems() {
	while (nDisplayItems) {
		displayItems[nDisplayItems-1]->task = NULL;
		nDisplayItems--;
	}
}

void DisposeWeaponStoreSprites() {
	int i;
	for (i=0;i<nWeapons;i++) items[i]->task = NULL;
	hilightSprite->task = NULL;
	RemoveDisplayItems();
	RunSAT(PrefFastDraw());
}

void DisposeShieldStoreSprites() {
	int i;
	for (i=0;i<nShields;i++) items[i]->task = NULL;
	hilightSprite->task = NULL;
	RemoveDisplayItems();
	RunSAT(PrefFastDraw());
}	

void DisposeEngineStoreSprites() {
	int i;
	for (i=0;i<nEngines;i++) items[i]->task = NULL;
	hilightSprite->task = NULL;
	RemoveDisplayItems();
	RunSAT(PrefFastDraw());
}

void DisposeStuffStoreSprites() {
	int i;
	for (i=0;i<nStuff;i++) items[i]->task = NULL;
	hilightSprite->task = NULL;
	RemoveDisplayItems();
	RunSAT(PrefFastDraw());
}

void DisposeFinanceStoreSprites() {
	int i;
	for (i=0;i<nFinance;i++) items[i]->task = NULL;
	hilightSprite->task = NULL;
	RemoveDisplayItems();
	RunSAT(PrefFastDraw());
}

void SetupMarketplaceSprites() {
	SpritePtr new;
	stores[0] = NewSprite(-2,gGameRect.right / 4-32, gGameRect.bottom / 4,SetupStore);
	stores[0]->mode = weaponStore; // weapons
	storeAngles[0] = 30;

	stores[1] = NewSprite(-2,gGameRect.right * 3 / 4-28, gGameRect.bottom / 4,SetupStore);
	stores[1]->mode = shieldStore; // shields
	storeAngles[1] = 6;

	stores[2] = NewSprite(-2,gGameRect.right/2-16 , gGameRect.bottom * 3 / 4,SetupStore);  
	stores[2]->mode = exitStore ; // exit
	storeAngles[2] = 18;
	exitSprite= 2;

	stores[3] = NewSprite(-2,0,0,SetupStore);
	stores[3]->mode = engineStore;
	storeAngles[3] = 24;

	stores[4] = NewSprite(-2,0,0,SetupStore);
	stores[4]->mode = stuffStore;
	storeAngles[4] = 12;

	stores[5] = NewSprite(-2,0,0,SetupStore);
	stores[5]->mode = financeStore;
	storeAngles[5] = 0;
	SetMarketplaceSpriteLocations();

}

void SetMarketplaceSpriteLocations() {
	stores[0]->position.h = gGameRect.right /4-32;
	stores[0]->position.v = gGameRect.bottom / 3 - 40;
	
	stores[1]->position.h = gGameRect.right *3/4-32;
	stores[1]->position.v =gGameRect.bottom / 3 -  40;
	
	stores[2]->position.h =gGameRect.right/2-16;
	stores[2]->position.v =gGameRect.bottom * 3 / 4;

	stores[3]->position.h = gGameRect.right / 4-32;
	stores[3]->position.v =gGameRect.bottom / 2;

	stores[4]->position.h = gGameRect.right * 3 / 4-32;
	stores[4]->position.v =gGameRect.bottom / 2;

	stores[5]->position.h = gGameRect.right/2 - 16;
	stores[5]->position.v = gGameRect.bottom / 8 - 10;

}

void GetRidOfStores() {
	int i;
	for (i=0;i<nNumStores;i++) {
		stores[i]->task = NULL;
	}
	RunSAT(PrefFastDraw());
}

void HideStoreSprites() {
	int i;
	for (i=0;i<nNumStores;i++) {
		if (stores[i]->mode != exitStore) {
			stores[i]->position.v -= 1000;
		}
	}
	RunSAT(PrefFastDraw());
	PeekOffscreen();
}

void ShowStoreSprites() {
	int i;
	for (i=0;i<nNumStores;i++) stores[i]->appLong = 0;
	SetMarketplaceSpriteLocations();
	RunSAT(PrefFastDraw());
	PeekOffscreen();
}

void DrawMarketplaceBackground() {
	GrafPtr oldPort;
	RGBColor c;
	Rect r;

	GetPort(&oldPort);
	SetPort(gSAT.backScreen);

	SetRect(&r,0,0,gSAT.offSizeH,gSAT.offSizeV);
	c.red= c.green = c.blue = 0;
	RGBForeColor(&c);
	PaintRect(&r);
	c.green = 32767;
	RGBForeColor(&c);
	PenSize(3,3);
	FrameRoundRect(&r,16,16);
	PenSize(1,1);
	RGBForeColor(&cBlack);									
	DrawStars();
	SetPort(oldPort);
	SATBackChanged(&r);

	RGBForeColor(&cBlack);									
}

Boolean HandleMoneyTransfer(PlayerShipData *p1dat,PlayerShipData *p2dat) {
	SpritePtr 	p1exit, p2exit;
	long 		l;
	int 		i;
	RGBColor 	c;
	GrafPtr 	oldPort;
	Boolean 	quitTrigger = FALSE;
	Str255		buffer;

	GetPort(&oldPort);
	SetPort(gSAT.backScreen);
	c.red=c.green=0;c.green = 65535;
	RGBForeColor(&c);
	TextFont(geneva);
	TextSize(9);
	TextFace(bold);
	GetIndString(buffer, rGeneralStrings, rExchangeCrystals);
	MoveTo(gGameRect.right/2 - StringWidth(buffer)/2,
				gGameRect.bottom-30);
	DrawString(buffer);
	RGBForeColor(&cBlack);									
	SetPort(oldPort);
	SATBackChanged(&gGameRect);
		
	p1ship->speed.h=p1ship->speed.v=0;
	p2ship->speed.h=p2ship->speed.v=0;
	p1ship->position.h -= 50;
	p2ship->position.h += 50;
	p1dat->shootMoney = TRUE;
	p2dat->shootMoney = TRUE;

	p1exit = NewSprite(-2,p1ship->position.h - 144,  p1ship->position.v-16,SetupStore);  
	p1exit->mode = exitStore;
	p1exit->appLong = 0;

	p2exit = NewSprite(-2,p2ship->position.h + 112,  p2ship->position.v-16,SetupStore);  
	p2exit->mode = exitStore;
	p2exit->appLong = 0;

	while (TRUE) {
		l=TickCount();
		RunSAT(PrefFastDraw());	
		if (CheckAnimationKeyBreaks()) {
			quitTrigger = TRUE;
			break;
		}
		if (p1ship->mode == 8 || p1ship->mode == 7 || p1ship->mode == 9) p1dat->noShieldLoss = FALSE;
		else p1dat->noShieldLoss = TRUE;

		if (p2ship->mode == 26 || p2ship->mode == 27) p2dat->noShieldLoss = FALSE;
		else p2dat->noShieldLoss = TRUE;
		
		if (p1ship->speed.h || p1ship->speed.v) {
			if (p1ship->mode == 26 || p1ship->mode == 27 || p1ship->mode == 25) {
				p1ship->speed.v = 0;			
			} else {
				p1ship->speed.v = 0;						
			}
		}
		if (p2ship->speed.h || p2ship->speed.v) {
			if (p2ship->mode == 8 || p2ship->mode == 7 || p2ship->mode == 9) {
				p2ship->speed.v = 0;			
			} else {
				p2ship->speed.v = 0;						
			}
		}

		if (p1dat->redrawInfo) RedrawPlayerShipInfo(p1dat);
		if (p2dat->redrawInfo) RedrawPlayerShipInfo(p2dat);


		if (p1exit->appLong || p2exit->appLong) {
			break;
		}
		while (l > TickCount() - GetPrefSpeed()) /*Maximize speed to 30 fps*/;		
	}

	FadeDown(20);

	p1exit->task = NULL;
	p2exit->task = NULL;
	p1ship->position.h = p1ship->position.v = -800;
	p2ship->position.h = p2ship->position.v = -800;
	
	SkipSAT();
	if (p1dat) p1dat->shootMoney = FALSE;
	if (p2dat) 	p2dat->shootMoney = FALSE;
	if (p1dat) 	p1dat->noShieldLoss = FALSE;
	if (p2dat) 	p2dat->noShieldLoss = FALSE;
	return quitTrigger;
}

Boolean CheckAnimationKeyBreaks() {
	unsigned char km[16];

	GetKeys( (long *) km);
	if (IsPressed(escapeKey,km)) {
		gameOver = TRUE;
		return TRUE;
	}
	if (IsPressed(capsLock,km)) {
		gameOver = HandlePause(TRUE);
		if (gameOver) return TRUE;
		if (p1Lives) {
			DrawScore((PlayerShipData *)p1ship->appPtr);
			DrawPlayerShields((PlayerShipData *)p1ship->appPtr);
			RedrawPlayerShipInfo((PlayerShipData *)p1ship->appPtr);
		}
		if (p2Lives) {
			DrawScore((PlayerShipData *)p2ship->appPtr);		
			RedrawPlayerShipInfo((PlayerShipData *)p2ship->appPtr);				
			DrawPlayerShields((PlayerShipData *)p2ship->appPtr);
		}
		DrawStructure();
	}
	return FALSE;
}