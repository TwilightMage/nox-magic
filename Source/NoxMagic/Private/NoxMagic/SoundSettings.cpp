#include "SoundSettings.h"

USoundSettings::USoundSettings()
{
	MasterVolume = 1.f;
	MusicVolume = 1.f;
	EffectsVolume = 1.f;
	AmbientVolume = 1.f;
}

void USoundSettings::SetMasterVolume(float volume)
{
	MasterVolume = volume;

	SaveConfig();
}

void USoundSettings::SetMusicVolume(float volume)
{
	MusicVolume = volume;

	SaveConfig();
}

void USoundSettings::SetEffectsVolume(float volume)
{
	EffectsVolume = volume;

	SaveConfig();
}

void USoundSettings::SetAmbientVolume(float volume)
{
	AmbientVolume = volume;

	SaveConfig();
}

USoundSettings* USoundSettings::GetSoundSettings()
{
	USoundSettings* result = NewObject<USoundSettings>();
	result->LoadConfig();
	return result;
}
