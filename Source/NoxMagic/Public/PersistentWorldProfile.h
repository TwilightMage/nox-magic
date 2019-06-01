#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SaveProfile.h"
#include "PersistentWorldProfile.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, Category = "NoxMagic")
class NOXMAGIC_API UPersistentWorldProfile : public UObject
{
	GENERATED_BODY()
	
private:
	FString profileName;

public:
	UFUNCTION(BlueprintPure, Category = "NoxMagic|Persistent world profile")
		FString GetName();

	UFUNCTION(BlueprintPure, Category = "NoxMagic|Persistent world profile", Meta = (DeterminesOutputType = "profileClass"))
		static UPersistentWorldProfile* GetWorldProfile(TSubclassOf<UPersistentWorldProfile> profileClass, FString profileName);

	UFUNCTION(BlueprintCallable, Category = "NoxMagic|Persistent world profile")
		void Save();

	UFUNCTION(BlueprintCallable, Category = "NoxMagic|Persistent world profile", Meta = (ExpandEnumAsExecs = "branch", DeterminesOutputType = "target"))
		UPersistentWorldProfile* Load(ELoadState& branch);
};
