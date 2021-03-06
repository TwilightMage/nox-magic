#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SoundSettings.generated.h"

UCLASS(Config=Sound)
class NOXMAGIC_API USoundSettings : public UObject
{
	GENERATED_BODY()
	
public:
	USoundSettings();

	float GetMasterVolume() const { return MasterVolume; }

	UFUNCTION(BlueprintSetter)
	void SetMasterVolume(float volume);

	float GetMusicVolume() const { return MusicVolume; }

	UFUNCTION(BlueprintSetter)
	void SetMusicVolume(float volume);

	float GetEffectsVolume() const { return EffectsVolume; }

	UFUNCTION(BlueprintSetter)
	void SetEffectsVolume(float volume);

	float GetAmbientVolume() const { return AmbientVolume; }

	UFUNCTION(BlueprintSetter)
	void SetAmbientVolume(float volume);

	UFUNCTION(BLueprintCallable)
	static USoundSettings* GetSoundSettings(class USoundMix* MasterMixer, class USoundMix* MusicMixer, class USoundMix* EffectsMixer, class USoundMix* AmbientMixer);

private:
	UPROPERTY(Config, BlueprintReadWrite, EditAnywhere, BlueprintSetter = SetMasterVolume, Meta = (AllowPrivateAccess = True))
	float MasterVolume;

	UPROPERTY(Config, BlueprintReadWrite, EditAnywhere, BlueprintSetter = SetMusicVolume, Meta = (AllowPrivateAccess = True))
	float MusicVolume;

	UPROPERTY(Config, BlueprintReadWrite, EditAnywhere, BlueprintSetter = SetEffectsVolume, Meta = (AllowPrivateAccess = True))
	float EffectsVolume;

	UPROPERTY(Config, BlueprintReadWrite, EditAnywhere, BlueprintSetter = SetAmbientVolume, Meta = (AllowPrivateAccess = True))
	float AmbientVolume;

	UPROPERTY()
	class USoundMix* MixerMaster;
	
	UPROPERTY()
	class USoundMix* MixerMusic;
	
	UPROPERTY()
	class USoundMix* MixerEffects;
	
	UPROPERTY()
	class USoundMix* MixerAmbient;
};
