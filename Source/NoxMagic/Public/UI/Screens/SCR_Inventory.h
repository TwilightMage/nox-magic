#pragma once

#include "CoreMinimal.h"
#include "UI/Screens/SCR_Screen.h"
#include "SCR_Inventory.generated.h"

UCLASS(Abstract)
class NOXMAGIC_API USCR_Inventory : public USCR_Screen
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void BindInventory(const FText& Title, class UInventory* Inventory);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void BindList(const FText& Title, const TArray<class UInventoryItem*>& Items);
};
