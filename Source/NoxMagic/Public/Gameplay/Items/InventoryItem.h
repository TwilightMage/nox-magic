#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InventoryItemDefaults.h"
#include "InventoryItem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryItemEvent, class UInventoryItem*, Sender);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInventoryItemSplited, class UInventoryItem*, Sender, class UInventoryItem*, AnotherPart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInventoryItemAbsorbed, class UInventoryItem*, Sender, class UInventoryItem*, Into);

UCLASS(Blueprintable)
class NOXMAGIC_API UInventoryItem : public UObject
{
	GENERATED_BODY()

public:
	FName GetRawID() const { return RawID; }
	FInventoryItemDefaults GetDefaults() const { return Defaults; }
	int GetCount() const { return Count; }
	float GetAdditiionalWeight() const { return AdditionalWeight;  }

	#pragma region UInventoryItem interface
	UFUNCTION(BlueprintNativeEvent, Category = "Character System|Inventory|Item")
	bool IsCompatibleWith(UInventoryItem* AnotherItem);

	virtual bool IsCompatibleWith_Implementation(UInventoryItem* AnotherItem);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Character System|Inventory|Item")
	UInventoryItem* Clone(int NewCount = -1);

	virtual UInventoryItem* Clone_Implementation(int NewCount);

	UFUNCTION(BlueprintNativeEvent, Category = "Character System|Inventory|Item")
	void Tick(float TimeDelta);

	virtual void Tick_Implementation(float TimeDelta) { };
	#pragma endregion

	UFUNCTION(BlueprintCallable, Category = World)
	static UInventoryItem* CreateItem(FName RawID, int Count = 1);

	UFUNCTION(BlueprintCallable, Category = "Character System|Inventory|Item")
	UInventoryItem* Split(int NewCount);

	UFUNCTION(BlueprintSetter, Category = "Character System|Inventory|Item")
	void SetCount(int NewValue);

	UFUNCTION(BlueprintCallable, Category = "Character System|Inventory|Item")
	void AddCount(int Amount);

	UFUNCTION(BlueprintSetter, Category = "Character System|Inventory|Item")
	void SetAdditiionalWeight(float NewAdditionalWeight);

	UFUNCTION(BlueprintCallable, Category = "Character System|Inventory|Item")
	void ChangeAdditionalWeight(float Delta);

	UFUNCTION(BlueprintCallable, Category = "Character System|Inventory|Item")
	bool Absorb(UInventoryItem* AnotherItem);

	UFUNCTION(BlueprintPure, Category = "Character System|Inventory|Item")
	static bool IsItemValid(UInventoryItem* item);

	UFUNCTION(BlueprintCallable, Category = "Character System|Inventory|Item")
	void SetNewOwner(UObject* NewOwner);

	UFUNCTION(BlueprintPure, Category = "Character System|Inventory|Item")
	float GetWeightForOne();

	UFUNCTION(BlueprintPure, Category = "Character System|Inventory|Item")
	float GetWeightTotal();

	UFUNCTION(BlueprintPure, Category = "Character System|Inventory|Item")
	static int CountItems(const TArray<UInventoryItem*> Items);

	UFUNCTION(BlueprintCallable, Category = "Character System|Inventory|Item")
	void RefreshDefaults();

	// Item container changed
	UPROPERTY(BlueprintAssignable, Meta = (ToolTip = "Item container changed"))
	FInventoryItemEvent OnOwnerChanged;

	//Item count updated
	UPROPERTY(BlueprintAssignable, Meta = (ToolTip = "Item count updated"))
	FInventoryItemEvent OnCountChanged;

	// Item count updated and new item instantiated
	UPROPERTY(BlueprintAssignable, Meta = (ToolTip = "Item count updated and new item instantiated"))
	FInventoryItemSplited OnSplited;

	// This item merged into another item
	UPROPERTY(BlueprintAssignable, Meta = (ToolTip = "This item merged into another item"))
	FInventoryItemAbsorbed OnAbsorbed;

	UPROPERTY(BlueprintAssignable)
	FInventoryItemEvent OnWeightChanged;

	virtual class UWorld* GetWorld() const override;

	void SetDefaults(const FInventoryItemDefaults& NewValue);

protected:
	UFUNCTION(BLueprintNativeEvent, Category = "Character System|Inventory|Item")
	void DefaultsLoaded();

	virtual void DefaultsLoaded_Implementation() { };

private:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (AllowPrivateAccess = true))
	FName RawID;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (AllowPrivateAccess = true))
	FInventoryItemDefaults Defaults;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, BlueprintSetter = SetCount, Meta = (AllowPrivateAccess = true))
	int Count;

	UPROPERTY(BLueprintReadWrite, EditAnywhere, BlueprintSetter = SetAdditiionalWeight, Meta = (AllowPrivateAccess = true))
	float AdditionalWeight;
};
