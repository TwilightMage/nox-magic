#include "InventoryItem.h"
#include "NMGameMode.h"

bool UInventoryItem::IsCompatibleWith_Implementation(UInventoryItem* AnotherItem)
{
	return RawID == AnotherItem->RawID;
}

UInventoryItem* UInventoryItem::Clone_Implementation(int NewCount)
{
	UInventoryItem* result = NewObject<UInventoryItem>(GetOuter(), Defaults.InstanceClass, RawID);
	result->RawID = RawID;
	result->Defaults = Defaults;
	result->Count = NewCount >= 0 ? NewCount : Count;
	return result;
}

UInventoryItem* UInventoryItem::CreateItem(FName RawID, int Count)
{
	if (auto itemDefaults = ANMGameMode::FetchItemDefaults(RawID))
	{
		UInventoryItem* result = NewObject<UInventoryItem>(GetTransientPackage(), itemDefaults->InstanceClass, *("InventoryItem_" + RawID.ToString() + "_Instance"));
		result->RawID = RawID;
		result->Count = Count;
		result->Defaults = *itemDefaults;
		result->DefaultsLoaded();

		return result;
	}

	return nullptr;
}

UInventoryItem* UInventoryItem::Split(int NewCount)
{
	if (NewCount > Count)
	{
		return nullptr;
	}

	UInventoryItem* clone = Clone(NewCount);

	SetCount(Count - NewCount);

	OnSplited.Broadcast(this, clone);

	return clone;
}

void UInventoryItem::SetCount(int NewValue)
{
	if (NewValue < 0) NewValue = 0;

	if (Count == NewValue) return;

	Count = NewValue;

	OnUpdated.Broadcast(this);
}

void UInventoryItem::AddCount(int Amount)
{
	SetCount(Count + Amount);
}

bool UInventoryItem::Merge(UInventoryItem* AnotherItem)
{
	if (IsCompatibleWith(AnotherItem) && Count + AnotherItem->Count <= Defaults.MaxStack)
	{
		SetCount(Count + AnotherItem->Count);
		AnotherItem->OnMerged.Broadcast(AnotherItem, this);
		AnotherItem->Count = 0;
		return true;
	}

	return false;
}

bool UInventoryItem::IsItemValid(UInventoryItem* item)
{
	return item != nullptr && item->IsValidLowLevel() && item->Count > 0;
}

void UInventoryItem::SetNewOwner(UObject* NewOwner)
{
	if (GetOuter() == NewOwner) return;

	LowLevelRename(*GetName(), NewOwner);

	OnOwnerChanged.Broadcast(this);
}

int UInventoryItem::CountItems(const TArray<UInventoryItem*> Items)
{
	int result = 0;

	for (auto item : Items)
	{
		if (item != nullptr && item->IsValidLowLevel())
		{
			result += item->GetCount();
		}
	}

	return result;
}

UWorld* UInventoryItem::GetWorld() const
{
	if (auto actor = GetTypedOuter<AActor>())
	{
		return actor->GetWorld();
	}
	else
	{
		return nullptr;
	}
}

void UInventoryItem::SetDefaults(const FInventoryItemDefaults & NewValue)
{
	Defaults = NewValue;
}