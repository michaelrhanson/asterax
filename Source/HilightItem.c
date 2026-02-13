/* Hilight item sprite for Asterax */

#include "SAT.h"
#include "AsteraxResources.h"
#include "Sprites.h"

FacePtr	hilightFace;

// appLong is used to detect hits

void InitHilight()
{
	hilightFace = GetFace(rHilightIcon);
}

pascal void SetupHilight (SpritePtr me)
{
	me->task = HandleHilight;
	me->hitTask = NULL;
	me->mode = 0;
	me->speed.h = 0;
	me->speed.v = 0;
	me->kind = 0;
	me->appLong = 0;
	SetRect(&me->hotRect, 0,0,0,0);
}

pascal void HandleHilight (SpritePtr me)
{
	me->face = hilightFace;
}
