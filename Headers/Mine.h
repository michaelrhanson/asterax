void InitMine();
pascal void SetupMine (SpritePtr me);
pascal void HandleMine (SpritePtr me);
pascal void HitMine (SpritePtr me, SpritePtr him);
PlayerShipData *GetMineFirer(SpritePtr me);
void SetMineFirer(SpritePtr me, PlayerShipData *data) ;
void PlayMinePing();
