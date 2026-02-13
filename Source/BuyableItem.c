/* Buyable item sprite for Asterax */

#include "SAT.h"
#include "AsteraxResources.h"
#include "Sprites.h"
#include "Game.fp"

#define rInsuranceFace	1510
#define rKachingSound	219

int itemCosts[nItems] = 
	{10,3,8,16,20,6,15,40,40,1,2,8,20,50,2,5,10,20,5,4,4,20,3,4,4,1,5,9,40,40,10};

Handle cantAffordSound, boughtItSound;

FacePtr	itemFaces[nItems];
FacePtr smallSpecialFaces[nStuff];
FacePtr unbuyableItemFaces[3];
FacePtr	insuranceFace;

// appLong is used to detect hits
int CalculateShipValue(PlayerShipData *ship);

extern int score;

void InitItems()
{
	int i;
	for(i=0;i<nItems;i++) itemFaces[i] = GetFace(rBaseItemIcon + i);
	for(i=0;i<nStuff;i++) smallSpecialFaces[i] = GetFace(rBaseSmallStuffIcon + i);
	unbuyableItemFaces[0] = GetFace(rBaseItemIcon + i4Shots);
	unbuyableItemFaces[1] = GetFace(rBaseItemIcon + iShieldsHalf);
	unbuyableItemFaces[2] = GetFace(rBaseItemIcon + iEngineHalf);
	insuranceFace = GetFace(rInsuranceFace);
	cantAffordSound = SATGetSound(235);
	boughtItSound = SATGetSound(rKachingSound);
}

void SetInsuranceCost(int amt) {
	itemCosts[iInsurance] = amt;
}

pascal void SetupInsuranceItem(SpritePtr me) {
	me->task = HandleInsuranceItem;
}

pascal void HandleInsuranceItem(SpritePtr me) {
	me->face = insuranceFace;
}

pascal void SetupItem (SpritePtr me) {
	me->task = HandleItem;
	me->hitTask = HitItem;

	me->mode = 0;
	me->speed.h = 0;
	me->speed.v = 0;
	
	me->kind = -2; /*Enemy kind ... collidable*/
	me->appLong = 0;
	SetRect(&me->hotRect, 4, 4, 28, 28);
}

pascal void HandleItem (SpritePtr me) {
	me->face = itemFaces[me->mode];
}

pascal void HitItem (SpritePtr me, SpritePtr him)
{
	int i,lim;
	PlayerShipData *data;
	
	if (me->task == NULL) return;
	if (him->task == HandlePlayerShip) {
		if (!me->appLong) {
			me->appLong = 1;
			data = (PlayerShipData *)him->appPtr;
			if (data->money < itemCosts[me->mode]) {
				// play a buzzer!
				PlaySound(kYummyChannel,cantAffordSound);
			} else {
				PlaySound(kYummyChannel,boughtItSound);
				data->money -= itemCosts[me->mode];				
				switch (me->mode) {
					case iFastShots:data->shotSpeed = 6;break;
					case i6Shots:data->maxNumberShots = 6;break;
					case i8Shots:data->maxNumberShots = 8;break;
					case i12Shots:data->maxNumberShots = 12;break;
					case iSafeShots:data->safeShots= TRUE;break;
					case iAutoShots:data->shotsPerBurst = 16;
									data->shotDelay = 3;break;
					case iDoubleBeamShots:data->shotsInOneShot = 2;break;
					case iTripleBeamShots:data->shotsInOneShot = 3;break;
					case iScatterShots:data->scatterShots =  TRUE;break;

					case iShieldRecharge:IncreaseShipShields(data, 1000);;break;
					case iShieldsOne:data->shieldMax = data->shields = 6000;break;
					case iShieldsTwo:data->shieldMax = data->shields = 8000;break;
					case iShieldsThree:data->shieldMax = data->shields = 12000;break;
					case iShieldsFour:data->shieldMax = data->shields = 16000;break;

					case iEngineOne:data->thrustRate=moveGranularity/8;data->topSpeed = 6*moveGranularity;break;
					case iEngineTwo:data->thrustRate=moveGranularity/5;data->topSpeed = 9*moveGranularity;break;
					case iEngineThree:data->thrustRate = moveGranularity/3;data->topSpeed =11*moveGranularity;break;
					case iEngineFour:data->thrustRate =moveGranularity/2;data->topSpeed = 13*moveGranularity;break;
					case iRetroEngine:data->retroRate = moveGranularity/8;break;

					case iStarburst:data->specials[data->nSpecials] = sStarburst;
									data->specialSelected = data->nSpecials;
									data->nSpecials++;
									break;	
					case iSuperShields:data->specials[data->nSpecials] = sSuperShields;
									data->specialSelected = data->nSpecials;
									data->nSpecials++;
									break;								
					case iGrabber:data->specials[data->nSpecials] = sGrabber;
									data->specialSelected = data->nSpecials;
									data->nSpecials++;
									break;								
					case iMine:data->specials[data->nSpecials] = sMine;
									data->specialSelected = data->nSpecials;
									data->nSpecials++;
									break;

					case iHyperspace:data->specials[data->nSpecials] = sHyperspace;
									data->specialSelected = data->nSpecials;
									data->nSpecials++;
									break;
					case iRecharge:data->specials[data->nSpecials] = sRecharge;
									data->specialSelected = data->nSpecials;
									data->nSpecials++;
									break;

					case iPoints:data->score+= 1000;
						 		 DrawScore(data);
								 break;

					case i5Points:data->score+=5000;
								DrawScore(data);
								break;
					case i10Points:data->score+=10000;
								DrawScore(data);
								break;
					case i50Points:data->score+=50000;
								DrawScore(data);
								break;
					case iNewShip:IncreasePlayerLives(data);
								data->redrawInfo = TRUE;
								break;
					case iInsurance:SetPlayerInsurance(data);
								break;

				}
			}
		}
	}
}


int GetItemCost(int i) {
	return itemCosts[i];
}

void PlotOneItem(int which, Point *p) {
	if (which == i4Shots) {
		SATPlotFaceToScreen(unbuyableItemFaces[0], *p, TRUE);
	} else if (which == iShieldsHalf) {
		SATPlotFaceToScreen(unbuyableItemFaces[1], *p, TRUE);
	} else if (which == iEngineHalf) {
		SATPlotFaceToScreen(unbuyableItemFaces[2], *p, TRUE);	
	} else {
		SATPlotFaceToScreen(itemFaces[which], *p, TRUE);
	}
}

void PlotSmallSpecialFace(int which, Point *p) {
	if (which < 0 || which >= nStuff) return;
	SATPlotFace(smallSpecialFaces[which], thePort, NULL, *p, FALSE);
}

void DisplayItemMessage(SpritePtr me, Rect *r) {
	Str255 buf;
	GetIndString(buf,rItemDescrips,me->mode+1);
	DrawStringInRect(buf,r);
}

void PlotAllItemsAndDescriptions(int whichType, Rect *r) {
	int start,end,i;
	Point p;
	Rect t;
	Str255 buf;

	switch (whichType) {
		case 0:start=0;end = nWeapons;break;
		case 1:start=nWeapons;end=nWeapons+nShields;break;
		case 2:start=nWeapons+nShields;end = nWeapons+nShields+nEngines;break;
		case 3:start=nWeapons+nShields+nEngines;end=nWeapons+nShields+nEngines+nStuff;
				r->top+=20;
				break;
		case 4:start=nWeapons+nShields+nEngines+nStuff;
				end = nWeapons+nShields+nEngines+nStuff+nFinance;break;
	}
	p.h = r->left;
	p.v = r->top;
	TextFace(0);
	
	for (i=start;i<end;i++) {
		RGBFore(0,0,0);
		PlotOneItem(i,&p);
		SetRect(&t,p.h + 40,p.v,p.h+(r->right-r->left)/2-10,p.v + 40);
		RGBFore(0,65535,65535);
		GetIndString(buf,rItemDescrips,i+1);
		DrawStringInRect(buf,&t);
		MoveTo(p.h,p.v + 40);
		RGBFore(0,65535,0);
		DrawString("\pCost: ");
		if (i == iInsurance) DrawString("\pVariable");
		else DrawInt(itemCosts[i]);
		p.v += 56;
		if (p.v > r->bottom - 56) {
			p.v = r->top;
			p.h += (r->right-r->left)/2;
		}
	}
	TextFace(bold);
}

int CalculateShipPremium(PlayerShipData *ship) {
	int val;
	val = CalculateShipValue(ship) / 6;
	if (val == 0) val++;
	return val;
}

int CalculateShipPayment(PlayerShipData *ship) {
	int val;
	val = CalculateShipValue(ship) / 10;
	if (val == 0) val++;
	return val;
}

int CalculateShipValue(PlayerShipData *data) {
	int value=0, i;
	switch (data->graphicID) {
		case 0:// crab
			if (data->maxNumberShots == 12) value+= itemCosts[i12Shots];
			if (data->shieldMax == 12000) value += itemCosts[iShieldsThree];
			else if (data->shieldMax == 16000) value += itemCosts[iShieldsFour];
			
			if (data->thrustRate == moveGranularity/8) value+=itemCosts[iEngineOne];
			else if (data->thrustRate == moveGranularity/5) value+=itemCosts[iEngineTwo];
			else if (data->thrustRate == moveGranularity/3) value+=itemCosts[iEngineThree];
			else if (data->thrustRate == moveGranularity/2) value+=itemCosts[iEngineFour];
			break;
		case 1:// mantis
			if (data->maxNumberShots == 6) value+= itemCosts[i6Shots];
			else if (data->maxNumberShots == 8) value+= itemCosts[i8Shots];
			else if (data->maxNumberShots == 12) value+= itemCosts[i12Shots];

			if (data->shieldMax == 6000) value += itemCosts[iShieldsOne];
			else if (data->shieldMax == 8000) value += itemCosts[iShieldsTwo];
			else if (data->shieldMax == 12000) value += itemCosts[iShieldsThree];
			else if (data->shieldMax == 16000) value += itemCosts[iShieldsFour];

			if (data->thrustRate == moveGranularity/3) value+=itemCosts[iEngineThree];
			else if (data->thrustRate == moveGranularity/2) value+=itemCosts[iEngineFour];
			break;			
		case 2:// moth
			if (data->maxNumberShots == 8) value+= itemCosts[i8Shots];
			else if (data->maxNumberShots == 12) value+= itemCosts[i12Shots];

			if (data->shieldMax == 8000) value += itemCosts[iShieldsTwo];
			else if (data->shieldMax == 12000) value += itemCosts[iShieldsThree];
			else if (data->shieldMax == 16000) value += itemCosts[iShieldsFour];

			if (data->thrustRate == moveGranularity/5) value+=itemCosts[iEngineTwo];
			else if (data->thrustRate == moveGranularity/3) value+=itemCosts[iEngineThree];
			else if (data->thrustRate == moveGranularity/2) value+=itemCosts[iEngineFour];
			break;
	}
	
	if (data->shotSpeed == 6) value += itemCosts[iFastShots];
	if (data->safeShots) value += itemCosts[iSafeShots];
	if (data->shotsPerBurst == 16) value += itemCosts[iAutoShots];
	if (data->shotsInOneShot == 2) value += itemCosts[iDoubleBeamShots];
	if (data->shotsInOneShot == 3) value += itemCosts[iTripleBeamShots];
	if (data->scatterShots) value += itemCosts[iScatterShots];

	for (i=0;i<data->nSpecials;i++) {
		switch (data->specials[i]) {
			case sStarburst:value += itemCosts[iStarburst];break;
			case sSuperShields:value += itemCosts[iSuperShields];break;
			case sGrabber:value += itemCosts[iGrabber];break;
			case sMine:value += itemCosts[iMine];break;
		}
	}
	return value;
}