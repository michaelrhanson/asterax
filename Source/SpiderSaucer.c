/* Spider Saucer sprite for Asterax */

#include "SAT.h"
#include "AsteraxResources.h"
#include "Sprites.h"
#include "Game.fp"
#include <math.h>

#define nSpiderSaucerFaces	18
FacePtr	spiderSaucerFaces[nSpiderSaucerFaces];

extern SpritePtr p1ship,p2ship;
extern int p1Lives, p2Lives;
extern Boolean playerOneDead, playerTwoDead;
extern Boolean levelOver;
extern int gLevel;

typedef struct {
	int direction;
	int health;
	int shotTimer;
	Boolean smartShots;
} SpiderSaucerData;

// appLong is used to store fractional position: hiword has v, loword has h

void InitSpiderSaucer()
{
	int i;
	for(i=0;i<nSpiderSaucerFaces;i++) spiderSaucerFaces[i] = GetFace(rBaseSpiderSaucerIcon + i);
}

pascal void SetupSpiderSaucer (SpritePtr me)
{
	long move;

	me->task = HandleSpiderSaucer;
	me->hitTask = HitSpiderSaucer;
	
	me->mode = 0;
	me->appPtr = NewPtr(sizeof(SpiderSaucerData));

	((SpiderSaucerData *)(me->appPtr))->direction = RangedRdm(0,4);
	((SpiderSaucerData *)(me->appPtr))->health = RangedRdm(6,8);
	((SpiderSaucerData *)(me->appPtr))->shotTimer = 5;
	if (gLevel >= 15) ((SpiderSaucerData *)(me->appPtr))->smartShots = TRUE;
	else ((SpiderSaucerData *)(me->appPtr))->smartShots = FALSE;
	
	PlayAlienAlertSound();
	switch (((SpiderSaucerData *)(me->appPtr))->direction) {
		case 0:me->position.h = 0;me->position.v = RangedRdm(30,gSAT.offSizeV-30);
			   me->speed.h = RangedRdm(moveGranularity/2,moveGranularity);
			   me->speed.v = 0;
			   break;
		case 1:me->position.h = RangedRdm(30,gSAT.offSizeH-30);
			   me->position.v = 0;
			   me->speed.v = RangedRdm(moveGranularity/2,moveGranularity);
			   me->speed.h = 0;
			   break;
		case 2:me->position.h = gSAT.offSizeH;
			   me->position.v = RangedRdm(30,gSAT.offSizeV-30);
			   me->speed.h = -RangedRdm(moveGranularity/2,moveGranularity);
			   me->speed.v = 0;
			   break;
		case 3:case 4:me->position.h = RangedRdm(30,gSAT.offSizeH-30);
			   me->position.v = gSAT.offSizeV;
			   me->speed.v = -RangedRdm(moveGranularity/2,moveGranularity);
			   me->speed.h = 0;
			   break;
	}
	me->kind = -3; /*Enemy kind*/
	me->appLong = 0;
	SetRect(&me->hotRect, 2, 6, 30, 26);
}

pascal void HandleSpiderSaucer (SpritePtr me)
{
	int shift,dh,dv;
	SpritePtr target,shot;
	Boolean goodTarget = TRUE;
	
	MoveSprite(me,0);

	(((SpiderSaucerData *)(me->appPtr))->shotTimer)--;
	if (((SpiderSaucerData *)(me->appPtr))->shotTimer==0) {
		if (p1Lives && p2Lives) {
			if (RangedRdm(0,400)>200) {
				target = p1ship;
				if (playerOneDead) goodTarget =FALSE;
			} else {
				target = p2ship;
				if (playerTwoDead) goodTarget =FALSE;
			}				
		} else if (p1Lives) {
			target = p1ship;
			if (playerOneDead) goodTarget =FALSE;
		} else if (p2Lives) {
			target = p2ship;
			if (playerTwoDead) goodTarget =FALSE;
		} else goodTarget = FALSE;

		if (goodTarget) {
			if (((SpiderSaucerData *)(me->appPtr))->smartShots) shot = NewSpriteAfter(me, 0, me->position.h+16, me->position.v+16, SetupEnemyShot_Seek);	
			else shot = NewSpriteAfter(me, 0, me->position.h+16, me->position.v+16, SetupEnemyShot);	
			
			shot->speed.h = (target->position.h - me->position.h)*moveGranularity/50;
			shot->speed.v = (target->position.v - me->position.v)*moveGranularity/50;
			{
				double a;
				a= sqrt((double)shot->speed.h*shot->speed.h + (double)shot->speed.v*shot->speed.v);
				shot->speed.h = shot->speed.h * (moveGranularity*7.0) / a;
				shot->speed.v = shot->speed.v * (moveGranularity*7.0) / a;
				if (shot->speed.h > moveGranularity * 7) shot->speed.h = moveGranularity*7;
				if (shot->speed.v > moveGranularity * 7) shot->speed.v = moveGranularity*7;
				if (shot->speed.h < - moveGranularity * 7) shot->speed.h = -moveGranularity*7;
				if (shot->speed.v < - moveGranularity * 7) shot->speed.v = -moveGranularity*7;
			}
					
			dh = (target->position.h) - me->position.h;
			dv = (target->position.v) - me->position.v;

			if (dh>50) me->speed.h = moveGranularity*2;
			else if (dh<-50) me->speed.h = -moveGranularity*2;
			else me->speed.h=0;
			
			if (dv>0) me->speed.v = moveGranularity*2;
			else if (dv<0) me->speed.v = -moveGranularity*2;
			else me->speed.v=0;
		}			
		if (RangedRdm(0,3)==2) (((SpiderSaucerData *)(me->appPtr))->shotTimer)=RangedRdm(30,40);
		else (((SpiderSaucerData *)(me->appPtr))->shotTimer) = RangedRdm(2,4);
	}
	
//	if ((me->position.h > gSAT.offSizeH-32) ||  (me->position.v > gSAT.offSizeV-32) ||
//			(me->position.h < 10) || (me->position.v < 0)) me->task = NULL;
	me->face = spiderSaucerFaces[me->mode/2];
	me->mode = me->mode + 1;
	if (levelOver) me->task = NULL;
	
	if (me->mode >= nSpiderSaucerFaces*2) me->mode = 0;
}

pascal void HitSpiderSaucer (SpritePtr me, SpritePtr him)
{
	int i,lim;
	if (me->task == NULL) return;
	if (him->task == HandlePlayerShot || him->task == HandlePlayerShip) {
		// it was a player shot.
		SpritePtr new;

		(((SpiderSaucerData *)(me->appPtr))->health)--;
		PlayAlienHitSound();

		if (((SpiderSaucerData *)(me->appPtr))->health == 0) {
			// he's dead.
			PlayShipExplodingSound();			
		 	NewSpriteAfter(me, 0, me->position.h, me->position.v, SetupExplosion);
			i = RangedRdm(4,8);
			while (i) {
			 	NewSpriteAfter(me, 0, me->position.h, me->position.v, SetupMoney);
				i--;
			}
			DisposePtr(me->appPtr);
			me->task = NULL;	
		}
			
		if (him->task == HandlePlayerShot) {
			((PlayerShipData *)him->appPtr)->shotsFired--;
			him->task = NULL;
			if (me->task == NULL) {
				((PlayerShipData *)him->appPtr)->score+= 1000;
				((PlayerShipData *)him->appPtr)->redrawScore = TRUE;
			}
		} else {
			DecreaseShipShields(((PlayerShipData *)him->appPtr), 1000);
			him->speed.h *= -1;
			him->speed.v *= -1;
		}
 	} else if (him->task == HandleMine) {
	 	PlayShipExplodingSound();
	 	NewSpriteAfter(me, 0, me->position.h, me->position.v, SetupExplosion);
	 	NewSpriteAfter(me, 0, me->position.h+RangedRdm(0,6), me->position.v+RangedRdm(0,6), SetupExplosion);
	 	NewSpriteAfter(me, 0, me->position.h-RangedRdm(0,6), me->position.v-RangedRdm(0,6), SetupExplosion);
		i = RangedRdm(4,8);
		while (i) {
		 	NewSpriteAfter(me, 0, me->position.h, me->position.v, SetupMoney);
			i--;
		}
		DisposePtr(me->appPtr);
		me->task = NULL;			
		him->task = NULL;
		GetMineFirer(him)->score+= 1000;
		GetMineFirer(him)->redrawScore = TRUE;
	}
}

void DrawOneSpiderSaucer(Point where) {
	SATPlotFaceToScreen(spiderSaucerFaces[0], where, TRUE);
}