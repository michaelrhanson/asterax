/* Money sprite for Asterax */

#include "SAT.h"
#include "AsteraxResources.h"
#include "Sprites.h"

#define nMoneyFaces	18
FacePtr	moneyFaces[nMoneyFaces];

Handle gotMoneySound;
extern Handle shotEnergySound;
extern Boolean levelOver;

// appLong is used to store fractional position: hiword has v, loword has h

void InitMoney()
{
	int i;
	for(i=0;i<nMoneyFaces;i++) moneyFaces[i] = GetFace(rBaseMoneyIcon + i);
	gotMoneySound = SATGetSound(rGotMoneySound);
}

pascal void SetupMoney (SpritePtr me) {
	me->task = HandleMoney;
	me->hitTask = HitMoney;
	me->mode = 0;
	me->speed.h = RangedRdm(-20,20);
	me->speed.v = RangedRdm(-20,20);
	
	me->kind = -1; /*Enemy kind -- can be destroyed by player shots*/
	SetRect(&me->hotRect, 4, 4, 12, 12);
	me->appLong = 0;
//	SATSoundPlay (energyAppearsSound,6,TRUE);

}

pascal void HandleMoney (SpritePtr me)
{
	MoveSprite(me,16);
	me->face = moneyFaces[me->mode/2];
	me->mode = me->mode + 1;
	
	if (me->mode >= nMoneyFaces*2) me->mode = 0;
	if (levelOver) me->task = NULL;
}

pascal void HitMoney (SpritePtr me, SpritePtr him)
{
	if (him->task == HandlePlayerShot) {
		if (!((PlayerShipData *)him->appPtr)->safeShots) {
			me->task = NULL;
		 	NewSpriteAfter(me, 0, me->position.h, me->position.v, SetupExplosion);
			PlaySound (kYummyChannel,shotEnergySound);
			((PlayerShipData *)him->appPtr)->shotsFired--;
			him->task = NULL;
		}
	} else if (him->task == HandlePlayerShip) {		
		if (!((PlayerShipData *)him->appPtr)->dead) {
			me->task = NULL;
			((PlayerShipData *)him->appPtr)->money++;		
			((PlayerShipData *)him->appPtr)->redrawInfo = TRUE;
			PlaySound (kYummyChannel,gotMoneySound);
		}
	}
}


void PlotOneMoney(Point *p) {
	SATPlotFaceToScreen(moneyFaces[2], *p, TRUE); /*Draw a Face (icon).*/
}

void PlayMoneySound(void) {
	PlaySound (kYummyChannel,gotMoneySound);
}