#include "NMCharacter.h"
#include "Interactive.h"
#include "Inventory.h"
#include "Drop.h"

#define LOCTEXT_NAMESPACE "NMCharacter"

ANMCharacter::ANMCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Interactive = CreateDefaultSubobject<UInteractive>(FName("Interactive"));
	Interactive->SetName(LOCTEXT("Talk", "Talk"));

	Inventory = CreateDefaultSubobject<UInventory>(FName("Inventory"));
}

void ANMCharacter::PickUp(ADrop* Drop)
{
	EInventoryAddItemResult AddItemResult;
	Inventory->AddItem(Drop->GetItem(), AddItemResult);
}
