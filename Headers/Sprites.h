#include "Sounds.h"
#include "Preferences.h"

#include "Explosion.h"
#include "SmallAsteroid.h"
#include "MedAsteroid.h"
#include "BigAsteroid.h"
#include "PlayerShip.h"
#include "ShieldHit.h"
#include "PlayerShot.h"
#include "EnergyCrystal.h"
#include "Money.h"
#include "CrystalGrabber.h"
#include "Mine.h"

#include "SimpleSaucer.h"
#include "EnemyShot.h"
#include "LeechSaucer.h"
#include "SpiderSaucer.h"
#include "Store.h"
#include "BuyableItems.h"
#include "HilightItem.h"

void MoveSprite(SpritePtr me,int size);
void DrawStringInRect(StringPtr buf,Rect *r);
void DrawCStringInRect(char *buf,Rect *r);
void GetAsteroidSpeed(Point *p);
void GetAsteroidSpeedFromBullet(Point *p, Point *bulletSpeed);

void CheckForCrystal(SpritePtr me);
void CheckForEnergy(SpritePtr me);

void RGBFore(unsigned int red,unsigned int green,unsigned int blue);


#define moveGranularity 64
