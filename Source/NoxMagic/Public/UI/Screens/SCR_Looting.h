#pragma once

#include "CoreMinimal.h"
#include "UI/Screens/SCR_Screen.h"
#include "SCR_Looting.generated.h"

UCLASS(Abstract)
class NOXMAGIC_API USCR_Looting : public USCR_Screen
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void BindInventoryToInventory(const FText& LeftTitle, class UInventory* LeftInventory, const FText& RightTitle, class UInventory* RightInventory);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void BindListToInventory(const FText& LeftTitle, const TArray<class UInventoryItem*>& LeftList, const FText& RightTitle, class UInventory* RightInventory);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void BindInventoryToList(const FText& LeftTitle, class UInventory* LeftInventory, const FText& RightTitle, const TArray<class UInventoryItem*>& RightList);
};
