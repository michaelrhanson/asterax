/* Small asteroid sprite for Asterax */

#include "SAT.h"
#include "AsteraxResources.h"
#include "Sprites.h"

#define nSmallAsteroidFaces	32
FacePtr	smallAsteroidFaces[nSmallAsteroidFaces];
Handle destroySmallRockSound;
extern int score;


// appLong is used to store fractional position: hiword has v, loword has h

void InitSmallAsteroid()
{
	int i;
	for(i=0;i<nSmallAsteroidFaces;i++) smallAsteroidFaces[i] = GetFace(rBaseSmallAsteroidIcon + i);
	destroySmallRockSound = SATGetSound(rDestroyBigRockSnd);
}

pascal void SetupSmallAsteroid (SpritePtr me) {
	me->task = HandleSmallAsteroid;
	me->hitTask = HitSmallAsteroid;

	me->mode = 0;
	me->appLong = 0;
	me->kind = -2; /*Enemy kind*/
	SetRect(&me->hotRect, 11, 11, 21, 21);
}

pascal void HandleSmallAsteroid (SpritePtr me)
{
	MoveSprite(me,16);
	me->face = smallAsteroidFaces[me->mode/2];
	me->mode = me->mode + 1;
	me->kind = -2; // strange "ghost-asteroid" bug... will this fix?
	if (me->mode >= nSmallAsteroidFaces*2) me->mode = 0;
}

pascal void HitSmallAsteroid (SpritePtr me, SpritePtr him)
{
	if (him->task == HandlePlayerShot) {
		me->task = NULL;
		PlaySound (kExplosionChannel,destroySmallRockSound);
	
		((PlayerShipData *)him->appPtr)->redrawScore = TRUE;
		((PlayerShipData *)him->appPtr)->shotsFired--;
		((PlayerShipData *)him->appPtr)->score +=30;
		him->task = NULL;
	 	NewSpriteAfter(me, 0, me->position.h, me->position.v, SetupExplosion);
	}
}

void DrawOneSmallRock(Point where) {
	SATPlotFaceToScreen(smallAsteroidFaces[0], where, TRUE);
}