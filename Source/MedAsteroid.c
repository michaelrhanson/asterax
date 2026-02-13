/* Mid asteroid sprite for Asterax */

#include "SAT.h"
#include "AsteraxResources.h"
#include "Sprites.h"

#define nMidAsteroidFaces	32
FacePtr	midAsteroidFaces[nMidAsteroidFaces];

// appLong is used to store fractional position: hiword has v, loword has h

Handle destroyMidRockSound;
extern int score;


void InitMidAsteroid()
{
	int i;
	for(i=0;i<nMidAsteroidFaces;i++) midAsteroidFaces[i] = GetFace(rBaseMidAsteroidIcon + i);
	destroyMidRockSound = SATGetSound(rDestroyBigRockSnd);
}

pascal void SetupMidAsteroid (SpritePtr me) {
	me->task = HandleMidAsteroid;
	me->hitTask = HitMidAsteroid;
	
	me->mode = 0;
	me->speed.h = RangedRdm(-20,20);
	me->speed.v = RangedRdm(-20,20);
	
	me->kind = -2; /*Enemy kind*/
	me->appLong = 0;
	SetRect(&me->hotRect, 6,6,26,26);
}

pascal void HandleMidAsteroid (SpritePtr me)
{
	MoveSprite(me,26);
	me->face = midAsteroidFaces[me->mode/3];
	me->mode = me->mode + 1;	
	if (me->mode >= nMidAsteroidFaces*3) me->mode = 0;
}

pascal void HitMidAsteroid (SpritePtr me, SpritePtr him)
{
	int i,lim;
	if (me->task == NULL) return;
	if (him->task == HandlePlayerShot) {
		SpritePtr new;

		lim = RangedRdm(2,4);
		for (i=0;i<lim;i++) {
		 	new = NewSpriteAfter(me, -2, me->position.h, me->position.v, SetupSmallAsteroid);
			if (him->task == HandlePlayerShip) {
				GetAsteroidSpeed(&new->speed);
				new->speed.h += him->speed.h/4;
				new->speed.v += him->speed.v/4;
			} else {
				GetAsteroidSpeedFromBullet(&new->speed,&him->speed);
			}
		}

		CheckForCrystal(me);
		CheckForEnergy(me);

		me->task = NULL;
		((PlayerShipData *)him->appPtr)->shotsFired--;
		((PlayerShipData *)him->appPtr)->score+=20;
		((PlayerShipData *)him->appPtr)->redrawScore = TRUE;
		him->task = NULL;

		NewSpriteAfter(me, 0, me->position.h, me->position.v, SetupExplosion);
		PlaySound (kExplosionChannel,destroyMidRockSound);
	}
}

void DrawOneMediumRock(Point where) {
	SATPlotFaceToScreen(midAsteroidFaces[0], where, TRUE);
}