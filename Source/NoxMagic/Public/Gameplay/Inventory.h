#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DefaultItem.h"
#include "Inventory.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInventoryMaxWeightChangedEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInventoryWeightChangedEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryAddedEvent, class UInventoryItem*, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryRemovedEvent, class UInventoryItem*, Item);

UENUM(BLueprintType)
enum class EInventoryAddItemByRawIDResult : uint8
{
	Success			UMETA(DisplayName = "Success"),
	NotEnoughSpace	UMETA(DisplayName = "Not enough space"),
	WrongRawID		UMETA(DisplayName = "Wrong raw ID")
};

UENUM(BLueprintType)
enum class EInventoryAddItemResult : uint8
{
	Success			UMETA(DisplayName = "Success"),
	NotEnoughSpace	UMETA(DisplayName = "Not enough space")
};

UENUM(BlueprintType)
enum class EInventoryTakeItemResult : uint8
{
	Success			UMETA(DisplayName = "Success"),
	NotEnough		UMETA(DisplayName = "Not enough")
};

UCLASS(ClassGroup=(Inventory), HideCategories = ("DoNotShow"), meta=(BlueprintSpawnableComponent))
class NOXMAGIC_API UInventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventory();

	float GetMaxWeight() const { return MaxWeight; }

	UPROPERTY(BlueprintAssignable, Category = "Character System|Inventory")
	FInventoryMaxWeightChangedEvent OnMaxWeightChanged;

	UPROPERTY(BlueprintAssignable, Category = "Character System|Inventory")
	FInventoryWeightChangedEvent OnWeightChanged;

	UPROPERTY(BlueprintAssignable, Category = "Character System|Inventory")
	FInventoryAddedEvent OnItemAdded;

	UPROPERTY(BlueprintAssignable, Category = "Character System|Inventory")
	FInventoryRemovedEvent OnItemRemoved;

	UFUNCTION(BlueprintCallable, Category = "Character System|Inventory")
	void SetMaxWeight(float NewMaxWeight);

	UFUNCTION(BlueprintCallable, Category = "Character System|Inventory")
	void IncrementMaxWeight(float Delta);

	UFUNCTION(BlueprintPure, Category = "Character System|Inventory")
	TArray<class UInventoryItem*> GetItems();

	UFUNCTION(BlueprintPure, Category = "Character System|Inventory")
	bool CanRecieveItemByRawID(FName RawID, int Count);

	UFUNCTION(BlueprintPure, Category = "Character System|Inventory")
	bool CanRecieveItem(class UInventoryItem* Target);

	UFUNCTION(BlueprintPure, Category = "Character System|Inventory")
	bool HaveItemByRawID(FName RawID, int Count);

	UFUNCTION(BlueprintPure, Category = "Character System|Inventory")
	bool HaveItemByPattern(class UInventoryItem* Pattern, int Count);

	UFUNCTION(BlueprintCallable, Category = "Character System|Inventory", Meta = (ExpandEnumAsExecs = Success))
	void TakeItemsByRawID(FName RawID, int Count, EInventoryTakeItemResult& Success, TArray<class UInventoryItem*>& TakenItems);

	UFUNCTION(BlueprintCallable, Category = "Character System|Inventory", Meta = (ExpandEnumAsExecs = Success))
	void TakeItemByRawID(FName RawID, int Count, EInventoryTakeItemResult& Success, class UInventoryItem*& TakenItem);

	UFUNCTION(BlueprintCallable, Category = "Character System|Inventory", Meta = (ExpandEnumAsExecs = Success))
	void TakeItemByPattern(UInventoryItem* Pattern, int Count, EInventoryTakeItemResult& Success, class UInventoryItem*& TakenItem);

	UFUNCTION(BLueprintPure, Category = "Character System|Inventory")
	TArray<class UInventoryItem*> FindSuitableItems(UPARAM(Meta = (BitMask, BitMaskEnum = EItemType)) int64 Flags);

	UFUNCTION(BlueprintCallable, Category = "Character System|Inventory", Meta = (ExpandEnumAsExecs = Success))
	void AddItemByRawID(FName RawID, int Count, EInventoryAddItemByRawIDResult& Success);

	UFUNCTION(BlueprintCallable, Category = "Character System|Inventory", Meta = (ExpandEnumAsExecs = Success))
	void AddItem(class UInventoryItem* ItemToAdd, EInventoryAddItemResult& Success);

	UFUNCTION(BlueprintPure, Category = "Character System|Inventory")
	float GetFreeWeight();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(BLueprintReadOnly, EditAnywhere, Category = "Character System|Inventory", Meta = (AllowPrivateAccess = True))
	float MaxWeight;

	UPROPERTY(EditAnywhere, EditFixedSize, Instanced, Meta = (AllowPrivateAccess = true))
	TArray<class UInventoryItem*> Items;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TSet<FName> DefaultItems;

	UFUNCTION(CallInEditor)
	void UpdateItems();

	// Item event handlers
	UFUNCTION()
	void ItemCountChanged(UInventoryItem* Sender);

	UFUNCTION()
	void ItemSplited(UInventoryItem* Sender, UInventoryItem* AnotherPart);

	UFUNCTION()
	void ItemAbsorbed(UInventoryItem* Sender, UInventoryItem* Into);

	UFUNCTION()
	void ItemOwnerChanged(UInventoryItem* Sender);

	UFUNCTION()
	void ItemWeightChanged(UInventoryItem* Sender);

	// Atomic functions to operate
	UFUNCTION(Server, Reliable)
	void IncrementItem_SERVER(int Index, int Count);

	UFUNCTION(NetMulticast, Reliable)
	void IncrementItem_MULTICAST(int Index, int Count);

	UFUNCTION(Server, Reliable)
	void AddItem_SERVER(class UInventoryItem* Item);

	UFUNCTION(NetMulticast, Reliable)
	void AddItem_MULTICAST(class UInventoryItem* Item);

	UFUNCTION(Server, Reliable)
	void RemoveItem_SERVER(int Index);

	UFUNCTION(NetMulticast, Reliable)
	void RemoveItem_MULTICAST(int Index);

	UFUNCTION(Server, Reliable)
	void SyncItem_SERVER(int Index, class UInventoryItem* Item);

	UFUNCTION(NetMulticast, Reliable)
	void SyncItem_MULTICAST(int Index, class UInventoryItem* Item);
};
