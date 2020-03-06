#include "Inventory.h"
#include "Drop.h"

UInventory::UInventory()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UInventory::PickUpItem_Implementation(ADrop* drop)
{
	AddItem(drop->item);
	drop->Disapear(inventoryColor);
}

void UInventory::AddItem(FItemContainer item)
{
	items.Add(item);
}
