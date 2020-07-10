#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySlot.generated.h"

UCLASS()
class NOXMAGIC_API UInventorySlot : public UUserWidget
{
	GENERATED_BODY()
	
public:
	class UInventory* GetInventory() const { return Inventory; }

	UFUNCTION(BlueprintSetter)
	void SetInventory(class UInventory* NewValue);

	int GetResponsiveIndex() const { return ResponsiveIndex; }

	UFUNCTION(BlueprintSetter)
	void SetResponsiveIndex(int NewValue);

	UFUNCTION(BlueprintPure)
	bool IsBound();

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void Update();

private:
	UPROPERTY(BlueprintReadWrite, BlueprintSetter = SetInventory, Meta = (AllowPrivateAccess = True, ExposeOnSpawn = True))
	class UInventory* Inventory;

	UPROPERTY(BlueprintReadWrite, BlueprintSetter = SetResponsiveIndex, Meta = (AllowPrivateAccess = True, ExposeOnSpawn = True))
	int ResponsiveIndex;

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = True))
	class UInventoryItem* CachedItem;

	UFUNCTION()
	void ItemChanged(int Index);

	UFUNCTION()
	void ItemUpdated(class UInventoryItem* Item);

	void RebindItemUpdated();
};
