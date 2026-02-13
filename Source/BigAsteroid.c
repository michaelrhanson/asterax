/* Big asteroid sprite for Asterax */

#include "SAT.h"
#include "AsteraxResources.h"
#include "Sprites.h"
#include "Game.fp"

#define nBigAsteroidFaces	32
FacePtr	asteroidFaces[nBigAsteroidFaces];
Handle destroyBigRockSound;
extern int score;

// appLong is used to store fractional position: hiword has v, loword has h

void InitBigAsteroid() {
	int i;
	for(i=0;i<nBigAsteroidFaces;i++) asteroidFaces[i] = GetFace(rBaseBigAsteroidIcon + i);
	destroyBigRockSound = SATGetSound(rDestroyBigRockSnd);
}

pascal void SetupBigAsteroid (SpritePtr me) {
	int maxV;

	me->task = HandleBigAsteroid;
	me->hitTask = HitBigAsteroid;

	me->mode = 0;
	GetAsteroidSpeed(&me->speed);
	me->kind = -2; /*Enemy kind*/
	me->appLong = 0;
	SetRect(&me->hotRect, 6, 6, 26, 26);
}

pascal void HandleBigAsteroid (SpritePtr me) {
	Point *p;
	MoveSprite(me,32);
	me->face = asteroidFaces[me->mode/3];
	me->mode = me->mode + 1;
	
	if (me->mode >= nBigAsteroidFaces*3) me->mode = 0;
}

pascal void HitBigAsteroid (SpritePtr me, SpritePtr him) {
	int i,lim;
	if (me->task == NULL) return;
	if (him->task == HandlePlayerShot) {
		SpritePtr new;

		lim = RangedRdm(2,4);
		for (i=0;i<lim;i++) {
		 	new = NewSpriteAfter(me, -2, me->position.h, me->position.v, SetupMidAsteroid);
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
		((PlayerShipData *)him->appPtr)->score+= 10;
		((PlayerShipData *)him->appPtr)->redrawScore = TRUE;
		him->task = NULL;
		PlaySound (kExplosionChannel,destroyBigRockSound);
	 	NewSpriteAfter(me, 0, me->position.h, me->position.v, SetupExplosion);
	}
}

void PlayDestroyBigRockSound(void) {
	PlaySound (kExplosionChannel,destroyBigRockSound);
}

void DrawOneBigRock(Point where) {
	SATPlotFaceToScreen(asteroidFaces[0], where, TRUE);
}