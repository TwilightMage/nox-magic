#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "InventoryItemDefaults.h"
#include "NMGameMode.generated.h"

UCLASS()
class NOXMAGIC_API ANMGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<class UDataContainer> ContainerType;

	UFUNCTION(BlueprintCallable, Category = "Inventory Subsystem", Meta = (DisplayName = "Fetch Item Defaults"))
	void K2_FetchItemDefaults(FName RawID, FInventoryItemDefaults& OutValue, bool& Success);

	static FInventoryItemDefaults* FetchItemDefaults(FName RawID);
};
