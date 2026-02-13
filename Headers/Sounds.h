#define kShotChannel		0
#define kExplosionChannel 	1
#define kYummyChannel		2
#define kBackgroundChannel	3

void InitSoundChannels() ;
void PlaySound(int whichChannel, Handle theSound) ;
void ShutUpChannel(int whichChannel);
void ShutUpAllChannels() ;
void PlayLevelOverSound() ;
void DontUseSounds(void);
Boolean UseSounds(void);
void SetVolZero(Boolean t);
