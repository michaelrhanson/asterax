void DetermineGameParameters(int nPlayers, int *p1type, int *p2type);
unsigned short RangedRdm( unsigned short min, unsigned short max);
void CreateP1Ship(void);
void CreateP2Ship(void);
void HandlePurchases() ;
void DisplayEndOfLevelMessage() ;
void DisplayBeginLevelMessage() ;
void SetupLevel(void) ;
void RedrawPlayerShipInfo(PlayerShipData *data);
void RedrawPlayerShipSpecial(PlayerShipData *data);
void DisplayStatusMessage(StringPtr buffer);
void DrawScore(PlayerShipData *data);
void DrawStars();
void RunGame(int nPlayers);
void RunCheatGame(int nPlayers, int startingLevel);
void ClearBackground();
void DrawPlayerShipRebuildingMessage(PlayerShipData *data);
PlayerShipData *GetPlayerInsurance (PlayerShipData *data);


Boolean HandleMoneyTransfer(PlayerShipData *p1dat,PlayerShipData *p2dat);

void DebugSet(int lev, int money);
