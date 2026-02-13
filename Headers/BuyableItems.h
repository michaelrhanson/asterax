void InitItems();
pascal void SetupItem (SpritePtr me);
pascal void HandleItem (SpritePtr me);
pascal void HitItem (SpritePtr me, SpritePtr him);
void PlotSmallSpecialFace(int which, Point *p);
void DisplayItemMessage(SpritePtr me, Rect *r);
void PlotOneItem(int which, Point *p);
void PlotAllItemsAndDescriptions(int whichType, Rect *r);
void SetInsuranceCost(int amt);

pascal void SetupInsuranceItem(SpritePtr me);
pascal void HandleInsuranceItem(SpritePtr me);

#define iFastShots 	0
#define i4Shots		-1
#define i6Shots		1
#define i8Shots		2
#define i12Shots	3
#define iAutoShots	4
#define iSafeShots	5
#define iDoubleBeamShots 6
#define iTripleBeamShots 7
#define iScatterShots	8

#define iShieldsHalf	-2
#define iShieldRecharge 9
#define iShieldsOne		10
#define iShieldsTwo		11
#define iShieldsThree	12
#define iShieldsFour	13

#define iEngineHalf		-3
#define iEngineOne		14
#define iEngineTwo		15
#define iEngineThree 	16
#define iEngineFour 	17
#define iRetroEngine  	18

#define iStarburst		19
#define iSuperShields	20
#define iGrabber		21
#define iMine			22
#define iHyperspace 	23
#define iRecharge		24

#define iPoints			25
#define i5Points		26
#define i10Points		27
#define i50Points		28
#define iNewShip		29
#define iInsurance		30

#define nWeapons 	9
#define nShields	5
#define nEngines 	5
#define nStuff		6
#define nFinance	6

#define nMaxItems 9
#define nItems (nWeapons+nShields+nEngines+nStuff+nFinance)
