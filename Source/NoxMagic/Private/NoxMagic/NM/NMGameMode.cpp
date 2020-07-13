#include "NMGameMode.h"
#include "NMDataContainer.h"
#include "NMGameMap.h"
#include "Engine/World.h"

void ANMGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (!UNMDataContainer::DoesContainerExists(ContainerClass))
	{
		UNMDataContainer::InitializeContainer(ContainerClass);
	}

	if (GameMapClass.Get())
	{
		GameMap = Cast<ANMGameMap>(GetWorld()->SpawnActor(GameMapClass));
	}
}

void ANMGameMode::K2_FetchItemDefaults(FName RawID, FInventoryItemDefaults& OutValue, bool& Success)
{
	if (auto Defaults = FetchItemDefaults(RawID))
	{
		OutValue = *Defaults;
		Success = true;
	}
	else
	{
		OutValue = FInventoryItemDefaults();
		Success = false;
	}
}

FInventoryItemDefaults* ANMGameMode::FetchItemDefaults(FName RawID)
{
	UDataTable* DataTable = LoadObject<UDataTable>(NULL, TEXT("/Game/Tweakable/DataTables/DT_ItemDefaults.DT_ItemDefaults"));

	ensure(DataTable != nullptr);

	if (DataTable != nullptr)
	{
		if (FInventoryItemDefaults* row = DataTable->FindRow<FInventoryItemDefaults>(RawID, ""))
		{
			return row;
		}
	}

	return nullptr;
}