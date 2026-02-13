/* Simple Saucer sprite for Asterax */

#include "SAT.h"
#include "AsteraxResources.h"
#include "Sprites.h"
#include "Game.fp"
#include <math.h>

#define nSimpleSaucerFaces	9
FacePtr	simpleSaucerFaces[nSimpleSaucerFaces];

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
} SaucerData;

// appLong is used to store fractional position: hiword has v, loword has h

void InitSimpleSaucer()
{
	int i;
	for(i=0;i<nSimpleSaucerFaces;i++) simpleSaucerFaces[i] = GetFace(rBaseSimpleSaucerIcon + i);
}

pascal void SetupSimpleSaucer (SpritePtr me)
{
	long move;

	me->task = HandleSimpleSaucer;
	me->hitTask = HitSimpleSaucer;
	me->mode = 0;
	me->appPtr = NewPtr(sizeof(SaucerData));

	((SaucerData *)(me->appPtr))->direction = RangedRdm(0,4);
	((SaucerData *)(me->appPtr))->health = RangedRdm(2,4);
	((SaucerData *)(me->appPtr))->shotTimer = 30;
	if (gLevel >= 10) ((SaucerData *)(me->appPtr))->smartShots = TRUE;
	else ((SaucerData *)(me->appPtr))->smartShots = FALSE;

	PlayAlienAlertSound();
	switch (((SaucerData *)(me->appPtr))->direction) {
		case 0:me->position.h = -24;
			   me->position.v = RangedRdm(30,gSAT.offSizeV-30);
			   me->speed.h = RangedRdm(moveGranularity/2,moveGranularity);
			   me->speed.v = 0;
			   break;
		case 1:me->position.h = RangedRdm(30,gSAT.offSizeH-30);
			   me->position.v = -24;
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

pascal void HandleSimpleSaucer (SpritePtr me)
{
	int shift;
	SpritePtr target,shot;
	Boolean goodTarget = TRUE;
	
	if (me->appPtr == NULL) {
		me->task = NULL;
		return;
	}
/*	switch (((SaucerData *)(me->appPtr))->direction) {
		case 0:if (me->position.h > gSAT.offSizeH-12) {
					me->task = NULL;return;
				}
				break;
		case 1:if (me->position.v > gSAT.offSizeV-12) {
					me->task = NULL;return;
				}
				break;
		case 2:if (me->position.h < -12) {
					me->task = NULL;return;
				}
				break;
		case 3:if (me->position.v < -12) {
					me->task = NULL;return;	
				}
				break;
	}
*/
	MoveSprite(me,0);
	if (RangedRdm(0,4)==2) {
		if (RangedRdm(0,20000)>10000) shift = moveGranularity/6;
		else shift = -moveGranularity/6;

		switch (((SaucerData *)(me->appPtr))->direction) {
			case 0:case 2:me->speed.v += shift;break;
			case 1:case 3:me->speed.h += shift;break;
		}
	}
	(((SaucerData *)(me->appPtr))->shotTimer)--;
	if (((SaucerData *)(me->appPtr))->shotTimer==0) {
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
		}

		if (goodTarget) {
			 if (((SaucerData *)(me->appPtr))->smartShots) shot = NewSpriteAfter(me, 0, me->position.h+16, me->position.v+16,  SetupEnemyShot_Seek);	
			else shot = NewSpriteAfter(me, 0, me->position.h+16, me->position.v+16,  SetupEnemyShot);	
			
			shot->speed.h = (target->position.h - me->position.h)*moveGranularity/50;
			shot->speed.v = (target->position.v - me->position.v)*moveGranularity/50;
			{
				double a;
				a= sqrt((double)shot->speed.h*shot->speed.h + (double)shot->speed.v*shot->speed.v);
				shot->speed.h = shot->speed.h * (moveGranularity*5.0) / a;
				shot->speed.v = shot->speed.v * (moveGranularity*5.0) / a;
				if (shot->speed.h > moveGranularity * 5)
					shot->speed.h = moveGranularity * 5;
				else if (shot->speed.h < -moveGranularity * 5)
					shot->speed.h = -moveGranularity * 5;

				if (shot->speed.v > moveGranularity * 5)
					shot->speed.v = moveGranularity * 5;
				else if (shot->speed.v <  -moveGranularity * 5)
					shot->speed.v = -moveGranularity * 5;
					
			}
		}
		(((SaucerData *)(me->appPtr))->shotTimer) = RangedRdm(20,40);
	}
	
	me->face = simpleSaucerFaces[me->mode/2];
	me->mode = me->mode + 1;
	if (levelOver) me->task = NULL;
	
	if (me->mode >= nSimpleSaucerFaces*2) me->mode = 0;

	if (me->position.v > gSAT.offSizeV || me->position.v < -32 || 
		me->position.h > gSAT.offSizeH  || me->position.h < -32) {
		me->task = NULL;
	}
}

pascal void HitSimpleSaucer (SpritePtr me, SpritePtr him)
{
	int i,lim;
	if (me->task == NULL) return;
	if (him->task == HandlePlayerShot || him->task == HandlePlayerShip) {
		// it was a player shot.
		SpritePtr new;
		PlayAlienHitSound();

		(((SaucerData *)(me->appPtr))->health)--;
		if (((SaucerData *)(me->appPtr))->health == 0) {
			// he's dead.
			PlayShipExplodingSound();
			
		 	NewSpriteAfter(me, 0, me->position.h, me->position.v, SetupExplosion);

			i = RangedRdm(0,2);
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
				((PlayerShipData *)him->appPtr)->score+= 500;
				((PlayerShipData *)him->appPtr)->redrawScore = TRUE;
			}
		} else {
			DecreaseShipShields(((PlayerShipData *)him->appPtr),1000);
			him->speed.h *= -1;
			him->speed.v *= -1;
		}
	} else if (him->task == HandleMine) {
		PlayShipExplodingSound();
		NewSpriteAfter(me, 0, me->position.h, me->position.v, SetupExplosion);
	 	NewSpriteAfter(me, 0, me->position.h+RangedRdm(0,6), me->position.v+RangedRdm(0,6), SetupExplosion);
	 	NewSpriteAfter(me, 0, me->position.h-RangedRdm(0,6), me->position.v-RangedRdm(0,6), SetupExplosion);
		i = RangedRdm(0,2);
		while (i) {
		 	NewSpriteAfter(me, 0, me->position.h, me->position.v, SetupMoney);
			i--;
		}
		DisposePtr(me->appPtr);
		me->task = NULL;			
		him->task = NULL;
		GetMineFirer(him)->score+= 500;
		GetMineFirer(him)->redrawScore = TRUE;
	}
}

void DrawOneSimpleSaucer(Point where) {
	SATPlotFaceToScreen(simpleSaucerFaces[0], where, TRUE);
}