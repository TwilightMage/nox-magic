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

	class ANMGameMap* GetGameMap() const { return GameMap; }

	UFUNCTION(BlueprintCallable, Category = "Inventory Subsystem", Meta = (DisplayName = "Fetch Item Defaults"))
	void K2_FetchItemDefaults(FName RawID, FInventoryItemDefaults& OutValue, bool& Success);

	static FInventoryItemDefaults* FetchItemDefaults(FName RawID);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Classes")
	TSubclassOf<class UNMDataContainer> ContainerClass;

	UPROPERTY(EditDefaultsOnly, Category = "Classes")
	TSubclassOf<class ANMGameMap> GameMapClass;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Meta = (AllowPrivateAccess = True))
	class ANMGameMap* GameMap;
};
