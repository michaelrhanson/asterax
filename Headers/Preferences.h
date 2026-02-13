#ifndef _H_PREFERENCES
#define _H_PREFERENCES

#include "PlayerShip.h"

void 		LoadPreferences();
void 		SetDefaultPrefs() ;
void 		UpdatePrefsFile(); 
void 		ClosePrefsFile() ;

Boolean 	DoScreenPrefsDialog(Boolean quitIfCancel) ;

void 		TogglePrefFastDraw();
void 		ChangeKey(char newkey, int player, int num) ;
void 		ChangePrefVolumeLevel(int delta);

Boolean 	PrefFastDraw(void);
Boolean 	GetPrefNewRules(void);
char 		GetPlayerKey(int player, int num) ;
int 		PrefDifficulty(void);
int 		GetPrefSpeed(void);
void 		TogglePrefNewRules(void);

void 		SetSuspendParameters(int level, long p1score, int p1lives, int p1money, PlayerShipData *p1ship, PlayerShipData *p1insur,long p2score, int p2lives, int p2money, PlayerShipData *p2ship, PlayerShipData *p2insur);
void 		GetSuspendParameters(int *level, long *p1score, int *p1lives, int *p1money, PlayerShipData **p1ship, PlayerShipData **p1insur,long *p2score, int *p2lives, int *p2money, PlayerShipData **p2ship, PlayerShipData **p2insur);

Boolean 	SuspendedGameAvailable();
void 		GetSuspendedSummary(int *level, int *nPlayers);
void 		LoadPlayerKeys(int player, PlayerShipData *data);

#define kBeginnerDif	0
#define kNormalDif		1
#define kExpertDif		2
#define kReallyExpertDif 3

#define kLeftPlayerKey		2
#define kRightPlayerKey		1
#define kThrustPlayerKey	0
#define kFirePlayerKey		3
#define kSelectPlayerKey	4
#define kActivatePlayerKey	5

#endif

