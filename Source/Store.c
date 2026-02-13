/* Store sprite for Asterax */

#include "SAT.h"
#include "AsteraxResources.h"
#include "Sprites.h"

FacePtr	storeFaces[nNumStores];

// appLong is used to detect  hits

extern int score;

void InitStores()
{
	int i;
	for(i=0;i<nNumStores;i++) storeFaces[i] = GetFace(rBaseStoreIcon + i);
}

pascal void SetupStore (SpritePtr me)
{
	me->task = HandleStore;
	me->hitTask = HitStore;
	me->mode = 0;
	me->speed.h = 0;
	me->speed.v = 0;
	
	me->kind = -2; /*Enemy kind ... collidable*/
	me->appLong = 0;
	SetRect(&me->hotRect, 8, 20, 54, 40);
}

pascal void HandleStore (SpritePtr me)
{
	me->face = storeFaces[me->mode];
}

pascal void HitStore (SpritePtr me, SpritePtr him)
{
	int i,lim;
	if (me->task == NULL) return;
	if (him->task == HandlePlayerShip) {
		me->appLong = 1;
	}

}
