#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SaveSlotMeta.h"
#include "SaveProfile.generated.h"

UENUM(BlueprintType)
enum class ELoadState : uint8
{
	Success,
	SlotDoesntExist
};

UCLASS(Blueprintable, Category = "NoxMagic")
class NOXMAGIC_API USaveProfile : public UObject
{
	GENERATED_BODY()

private:
	FString profileName;
	
public:
	UFUNCTION(BlueprintPure, Category = "NoxMagic|Save profile")
		FString GetName();

	UFUNCTION(BlueprintPure, Category = "NoxMagic|Save profile", Meta = (DeterminesOutputType = "profileClass"))
		static USaveProfile* GetSaveProfile(TSubclassOf<USaveProfile> profileClass, FString profileName);

	UFUNCTION(BlueprintCallable, Category = "NoxMagic|Save profile")
		void QuickSave(int savegameBufferSize, UTexture2D* thumb);

	UFUNCTION(BlueprintCallable, Category = "NoxMagic|Save profile", Meta = (ExpandEnumAsExecs = "branch", DeterminesOutputType = "target"))
		USaveProfile* QuickLoad(ELoadState& branch);

	UFUNCTION(BlueprintCallable, Category = "NoxMagic|Save profile")
		void Save(FString slotName, UTexture2D* thumb);

	UFUNCTION(BlueprintCallable, Category = "NoxMagic|Save profile", Meta = (ExpandEnumAsExecs = "branch", DeterminesOutputType = "target"))
		USaveProfile* Load(FString slotName, ELoadState& branch);

	UFUNCTION(BlueprintCallable, Category = "NoxMagic|Save profile")
		static bool DoesProfileExists(FString profileName);

	UFUNCTION(BlueprintCallable, Category = "NoxMagic|Save profile")
		bool DoesSlotExists(FString slotName);

	UFUNCTION(BlueprintCallable, Category = "NoxMagic|Save profile")
		TArray<FSaveMeta> GetSlots();

	UFUNCTION(BlueprintCallable, Category = "NoxMagic|Save profile")
		static TArray<FString> GetProfiles();

	UFUNCTION(BlueprintCallable, Category = "NoxMagic|Save profile")
		void ClearProfile();
};
