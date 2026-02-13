/* Leech Saucer sprite for Asterax */

#include "SAT.h"
#include "AsteraxResources.h"
#include "Sprites.h"
#include "Game.fp"
#include <math.h>

#define nLeechSaucerFaces	9

#define rLeechAppearsSound	221

FacePtr	leechSaucerFaces[nLeechSaucerFaces];

extern SpritePtr p1ship,p2ship;
extern int p1Lives, p2Lives, gNPlayers;
extern Boolean playerOneDead, playerTwoDead;
extern Boolean levelOver;

Handle pulseSound, leechAppearsSound;

typedef struct {
	int direction;
	int health;
	int shotTimer;
	int lifespan;
} SaucerData;

// appLong is used to store fractional position: hiword has v, loword has h

static Boolean anyLeeches;

void InitLeechSaucer() {
	int i;
	for(i=0;i<nLeechSaucerFaces;i++) leechSaucerFaces[i] = GetFace(rBaseLeechSaucerIcon + i);
	pulseSound = SATGetSound(220);
	leechAppearsSound = SATGetSound(rLeechAppearsSound);
	anyLeeches = FALSE;
}

pascal void SetupLeechSaucer (SpritePtr me) {
	long move;
	
	me->task = HandleLeechSaucer;
	me->hitTask = HitLeechSaucer;
	
	me->mode = 0;
	me->appPtr = NewPtr(sizeof(SaucerData));

	((SaucerData *)(me->appPtr))->health = RangedRdm(6,8);
	((SaucerData *)(me->appPtr))->shotTimer = 0;

	((SaucerData *)(me->appPtr))->direction = RangedRdm(0,4);
	((SaucerData *)(me->appPtr))->lifespan = RangedRdm(600,1200);

	PlaySound (kBackgroundChannel,leechAppearsSound);
	switch (((SaucerData *)(me->appPtr))->direction) {
		case 0:me->position.h = 20;me->position.v = RangedRdm(30,gSAT.offSizeV-30);
			   me->speed.h = 0;
			   me->speed.v = 0;
			   break;
		case 1:me->position.h = RangedRdm(30,gSAT.offSizeH-30);
			   me->position.v = 20;
			   me->speed.v = 0;
			   me->speed.h = 0;
			   break;
		case 2:me->position.h = gSAT.offSizeH-20;
			   me->position.v = RangedRdm(30,gSAT.offSizeV-30);
			   me->speed.h = 0;
			   me->speed.v = 0;
			   break;
		case 3:case 4:me->position.h = RangedRdm(30,gSAT.offSizeH-30);
			   me->position.v = gSAT.offSizeV-20;
			   me->speed.v = 0;
			   me->speed.h = 0;
			   break;
	}
	me->kind = -3; /*Enemy kind*/
	me->appLong = 0;
	SetRect(&me->hotRect, 2, 6, 30, 26);
	anyLeeches = TRUE;
}

pascal void HandleLeechSaucer (SpritePtr me) {
	int shift; 
	SpritePtr target;
	int dh,dv;
	SaucerData *data;

	data = (SaucerData *)me->appPtr;
	me->face = leechSaucerFaces[me->mode/2];
	me->mode = me->mode + 1;
	if (levelOver) {
		me->task = NULL;	
		anyLeeches = FALSE;
	}
	if (me->mode == nLeechSaucerFaces*2) PlaySound(kBackgroundChannel,pulseSound);
	if (me->mode >= nLeechSaucerFaces*2) me->mode = 0;
	
	if (data->lifespan>0) MoveSprite(me,32);
	else {
		MoveSprite(me,0);
		if (me->position.h < -16 || me->position.v < -16 ||
			me->position.h > gSAT.offSizeH || me->position.v > gSAT.offSizeV) {
				me->task = NULL;
				anyLeeches = FALSE;
				return;
			}
	}
	anyLeeches = TRUE;
	data->lifespan --;
	if (p1Lives && p2Lives) {
		if (playerOneDead && playerTwoDead) return;
		else if (playerOneDead) target = p2ship;
		else if (playerTwoDead) target = p1ship;
		else {
			if (RangedRdm(0,400)>200) target = p1ship;
			else target = p2ship;
		}
	} else if (p1Lives) {
		if (playerOneDead) return;
		target = p1ship;
	} else if (p2Lives) {
		if (playerTwoDead) return;
		target = p2ship;
	} else return;

	dh = (target->position.h - me->position.h);
	dv = (target->position.v - me->position.v);
	if (dh*dh + dv*dv < 22500) {
		if (ABS(dh) < 150) {
			if (dh) me->speed.h -= moveGranularity/2 * (dh)/ABS(dh);
		} else {
			if (me->speed.h) me->speed.h -= (ABS(me->speed.h)/me->speed.h)*moveGranularity/4;	
		}
		if (ABS(dv) < 150) {
			if (dv) me->speed.v -= moveGranularity/2 * (dv)/ABS(dv);
		} else {
			if (me->speed.v) me->speed.v -= (ABS(me->speed.v)/me->speed.v)*moveGranularity/4;
		}
	}
	if (me->position.h < 0 || me->position.h > gSAT.offSizeH - 32) {
		if (ABS(me->speed.h) < 10) me->speed.h += 30;
	}
	if (me->position.v < 0 || me->position.v > gSAT.offSizeV - 32) {
		if (ABS(me->speed.v) < 10) me->speed.v += 30;
	}
	
	if (target->task) {
		if (ShipShields((PlayerShipData *)target->appPtr) > 3) {
			DecreaseShipShields(((PlayerShipData *)target->appPtr),3);
		}
	}
}

pascal void HitLeechSaucer (SpritePtr me, SpritePtr him)
{
	int i,lim;
	if (me->task == NULL) return;
	if (him->task == HandlePlayerShot || him->task == HandlePlayerShip) {
		// it was a player shot.
		SpritePtr new;

		(((SaucerData *)(me->appPtr))->health)--;
		PlayAlienHitSound();
		
		if (((SaucerData *)(me->appPtr))->health == 0) {
			// he's dead.
			PlayShipExplodingSound();
		 	NewSpriteAfter(me, 0, me->position.h, me->position.v, SetupExplosion);
			i = RangedRdm(2,4);
			while (i) {
			 	NewSpriteAfter(me, 0, me->position.h, me->position.v, SetupMoney);
				i--;
			}

			DisposePtr(me->appPtr);
			me->task = NULL;	
			ResetAnyLeeches();

		}
			
		if (him->task == HandlePlayerShot) {
			((PlayerShipData *)him->appPtr)->shotsFired--;
			him->task = NULL;
			if (me->task == NULL) {
				((PlayerShipData *)him->appPtr)->score+= 750;
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
		i = RangedRdm(2,4);
		while (i) {
		 	NewSpriteAfter(me, 0, me->position.h, me->position.v, SetupMoney);
			i--;
		}

		DisposePtr(me->appPtr);
		me->task = NULL;			
		him->task = NULL;

		GetMineFirer(him)->score+= 750;
		GetMineFirer(him)->redrawScore = TRUE;
		ResetAnyLeeches();
	}
}

Boolean AreAnyLeeches() {
	return anyLeeches;
}

void ResetAnyLeeches() {
	anyLeeches = FALSE;
}

void DrawOneLeech(Point where) {
	SATPlotFaceToScreen(leechSaucerFaces[0], where, TRUE);
}