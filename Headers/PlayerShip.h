#ifndef _H_PLAYERSHIP
#define _H_PLAYERSHIP
#include "SAT.h"
#include "Equipment.h"

void InitPlayerShip();
pascal void SetupPlayerShip (SpritePtr me);
pascal void HandlePlayerShip (SpritePtr me);
pascal void HitPlayerShip (SpritePtr me, SpritePtr him);


#define IsPressed(k,km) (((km[k>>3]>>(k&7))&1))

#define kMaxSpecials 40
typedef enum {sStarburst, sSuperShields, sGrabber, sMine, sHyperspace, sRecharge} specialStuff;

typedef struct {
	Point 	fracLoc;
	int		graphicID;
	int		pnum;

// shot-data:
//	  customizable:
	int 	maxNumberShots;
	int		shotDelay;
	int		shotsPerBurst;
	int		shotsInOneShot;
	int 	shotSpeed;
	int 	shotRange;
	int		safeShots;
	Boolean	scatterShots;
//    utility:
	long  	shotTime;
	int   	shotsFired;
	int		shotsFiredInBurst;

// shield-data:
// 	   customizable:
	int		shieldMax;
	int		decayRate; // hmph.  Use this?  Need a decay counter, then.
// utility:
	int		levelStartCount;
	int 	shields;

// engine-data:
//   customizable:
	int 	thrustRate;
	long	topSpeed;
	int		turnRate;
	int		retroRate;
// utility:
	int		turnCount;
	int		thrustSoundCount;
	
// "stuff" data:
	int 	whichSpecialActive;
	union {
		struct {
			enum {starBurstBegin, starBurstGo, starBurstEnd} starBurstPhase;
			int	starBurstTimer;
		} starBurst;
		struct {
			SpritePtr theShieldSprite;
			int superShieldTimer;
		} superShield;
		struct {
			SpritePtr grabberSprite;
			Boolean   stillOut;
		} grabber;
		struct {
			int theTime;
		} timer;
	} specialData;
	

	long	score;

	int		money;
	int		value;
//	int		*nLives;
	
	Boolean	redrawInfo, redrawScore, redrawSpecial;
	Boolean	shotKeyRelease;
	int		selSpecialPhase;

	specialStuff specials[kMaxSpecials];
	int		specialSelected, nSpecials;
	
	int 	leftKey, rightKey, thrustKey, fireKey, selSpecialKey, actSpecialKey;
	Boolean suspend,noShieldLoss,noThrust,noControl;
	Boolean shootMoney;
	
	Boolean dead;
	Point   target;
} PlayerShipData;

PlayerShipData *SetUpShip(int type);
void AddEquipmentToShip(PlayerShipData *ship, int itemType, int itemCode);
void GetEquipmentGameEffect(StringPtr str,int itemType,int itemCode);
void DecreaseShipShields(PlayerShipData *data, int howMuch);
void IncreaseShipShields(PlayerShipData *data, int howMuch);
void IncreaseShipMoney(PlayerShipData *data, int howMuch);
void PlotOneShip(int which, Point *p);
void DrawShipAndName(int which, Point *p);
int CalculateShipPremium(PlayerShipData *ship);
int CalculateShipPayment(PlayerShipData *ship);

int ShipShields(PlayerShipData *data);


// Puppet Sprite routines

pascal void SetupShipPuppet (SpritePtr me);
pascal void HandleShipPuppet (SpritePtr me);

#endif