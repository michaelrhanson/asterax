/* Prototypes for Marketplace*/

void HandlePurchases();
void HandleOnePlayer(SpritePtr spr, PlayerShipData *ship);
void RunStoreAnimation(int whichStore, SpritePtr spr, PlayerShipData *ship);
void FlyShipToSprite(SpritePtr spr, PlayerShipData *ship, SpritePtr target);
void FlyShipToCenter(SpritePtr spr, PlayerShipData *ship);
void HilightClosest(SpritePtr spr, int *closest, int *oldclosest);
void SetupStoreSprites(int start, int end);
void CreateNewDisplayItem(Point *p, int mode, Boolean insured);
void DrawCurrentEquipmentMessage(Point *p);

void DrawWeaponStoreCurrentEquipment(PlayerShipData *data, Point *p, PlayerShipData *ins);
void DrawShieldStoreCurrentEquipment(PlayerShipData *data,	Point *p, PlayerShipData *ins);
void DrawEngineStoreCurrentEquipment(PlayerShipData *data,Point *p, PlayerShipData *ins);
void DrawStuffStoreCurrentEquipment(PlayerShipData *data, Point *p, PlayerShipData *ins);

void SetupWeaponStoreSprites(PlayerShipData *data);
void SetupShieldStoreSprites(PlayerShipData *data);
void SetupEngineStoreSprites(PlayerShipData *data);
void SetupStuffStoreSprites(PlayerShipData *data);
void SetupFinanceStoreSprites(PlayerShipData *data);
void RemoveDisplayItems();
void DisposeWeaponStoreSprites();
void DisposeShieldStoreSprites();
void DisposeEngineStoreSprites();
void DisposeStuffStoreSprites();
void DisposeFinanceStoreSprites();
void SetupMarketplaceSprites();
void SetMarketplaceSpriteLocations();
void GetRidOfStores();
void HideStoreSprites();
void ShowStoreSprites();
void DrawMarketplaceBackground();
Boolean HandleMoneyTransfer(PlayerShipData *p1dat,PlayerShipData *p2dat);
Boolean CheckAnimationKeyBreaks(); 