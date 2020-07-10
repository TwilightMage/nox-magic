#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryGrid.generated.h"

UCLASS()
class NOXMAGIC_API UInventoryGrid : public UUserWidget
{
	GENERATED_BODY()
	
public:
	class UInventory* GetInventory() const { return Inventory; }

	UFUNCTION(BlueprintPure, BlueprintImplementableEvent)
	class UGridPanel* GetGrid();

protected:
	virtual void NativePreConstruct() override;

private:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Meta = (AllowPrivateAccess = True))
	class UInventory* Inventory;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Meta = (AllowPrivateAccess = True))
	TSubclassOf<class UInventorySlot> SlotClass;
};
