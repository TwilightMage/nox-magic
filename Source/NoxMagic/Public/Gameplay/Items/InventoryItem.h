#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InventoryItemDefaults.h"
#include "InventoryItem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryItemEvent, class UInventoryItem*, Sender);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInventoryItemSplited, class UInventoryItem*, Sender, class UInventoryItem*, AnotherPart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInventoryItemMerged, class UInventoryItem*, Sender, class UInventoryItem*, Into);

UCLASS(Blueprintable)
class NOXMAGIC_API UInventoryItem : public UObject
{
	GENERATED_BODY()

public:
	FORCEINLINE FName GetRawID() const { return RawID; }
	FORCEINLINE FInventoryItemDefaults GetDefaults() const { return Defaults; }
	FORCEINLINE int GetCount() const { return Count; }

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

	UFUNCTION(BlueprintCallable, Category = "Character System|Inventory|Item")
	bool Merge(UInventoryItem* AnotherItem);

	UFUNCTION(BlueprintPure, Category = "Character System|Inventory|Item")
	static bool IsItemValid(UInventoryItem* item);

	UFUNCTION(BlueprintCallable, Category = "Character System|Inventory|Item")
	void SetNewOwner(UObject* NewOwner);

	UFUNCTION(BlueprintPure, Category = "Character System|Inventory|Item")
	static int CountItems(const TArray<UInventoryItem*> Items);

	// Item container changed
	UPROPERTY(BlueprintAssignable)
	FInventoryItemEvent OnOwnerChanged;

	//Item count updated
	UPROPERTY(BlueprintAssignable)
	FInventoryItemEvent OnUpdated;

	// Item count updated and new item instantiated
	UPROPERTY(BlueprintAssignable)
	FInventoryItemSplited OnSplited;

	// This item merged into another item
	UPROPERTY(BlueprintAssignable)
	FInventoryItemMerged OnMerged;

	virtual class UWorld* GetWorld() const override;

	void SetDefaults(const FInventoryItemDefaults& NewValue);

protected:
	UFUNCTION(BLueprintNativeEvent, Category = "Character System|Inventory|Item")
	void DefaultsLoaded();

	virtual void DefaultsLoaded_Implementation() { };

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FName RawID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FInventoryItemDefaults Defaults;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintSetter = SetCount, meta = (AllowPrivateAccess = true))
	int Count;
};
