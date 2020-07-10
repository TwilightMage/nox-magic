#include "InventorySlot.h"
#include "Inventory.h"
#include "InventoryItem.h"

void UInventorySlot::SetInventory(UInventory* NewValue)
{
	if (Inventory)
	{
		Inventory->OnItemChanged.RemoveDynamic(this, &UInventorySlot::ItemChanged);
	}

	if (NewValue)
	{
		NewValue->OnItemChanged.AddDynamic(this, &UInventorySlot::ItemChanged);
	}

	Inventory = NewValue;

	if (IsBound())
	{
		RebindItemUpdated();
	}
}

void UInventorySlot::SetResponsiveIndex(int NewValue)
{

}

bool UInventorySlot::IsBound()
{
	return Inventory && Inventory->IsValidLowLevel() && ResponsiveIndex >= 0 && Inventory->GetSize() >= ResponsiveIndex;
}

void UInventorySlot::ItemChanged(int Index)
{
	if (Index == ResponsiveIndex)
	{
		RebindItemUpdated();
	}
}

void UInventorySlot::ItemUpdated(UInventoryItem* Item)
{
	Update();
}

void UInventorySlot::RebindItemUpdated()
{
	if (CachedItem)
	{
		CachedItem->OnUpdated.RemoveDynamic(this, &UInventorySlot::ItemUpdated);
	}

	CachedItem = nullptr;

	if (auto NewItem = Inventory->GetItemAt(ResponsiveIndex))
	{
		NewItem->OnUpdated.AddDynamic(this, &UInventorySlot::ItemUpdated);
		CachedItem = NewItem;
	}

	Update();
}
