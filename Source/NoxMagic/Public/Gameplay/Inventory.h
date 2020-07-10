#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DefaultItem.h"
#include "Inventory.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInventoryResizedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemChangedDelegate, int, index);

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

	UFUNCTION(BlueprintPure, Category = "Character System|Inventory")
	int GetSize() const;

	UFUNCTION(BlueprintCallable, Category = "Character System|Inventory")
	void SetSize(int NewSize);

	UFUNCTION(BlueprintCallable, Category = "Character System|Inventory")
	void IncrementSize(int Delta);

	UFUNCTION(BlueprintPure, Category = "Character System|Inventory")
	class UInventoryItem* GetItemAt(int Index);

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
	void TakeItemAt(int Index, int Count, EInventoryTakeItemResult& Success, class UInventoryItem*& TakenItem);

	UFUNCTION(BlueprintCallable, Category = "Character System|Inventory", Meta = (ExpandEnumAsExecs = Success))
	void TakeItemByRawID(FName RawID, int Count, EInventoryTakeItemResult& Success, class UInventoryItem*& TakenItem);

	UFUNCTION(BlueprintCallable, Category = "Character System|Inventory", Meta = (ExpandEnumAsExecs = Success))
	void TakeItemByPattern(UInventoryItem* Pattern, int Count, EInventoryTakeItemResult& Success, class UInventoryItem*& TakenItem);

	UFUNCTION(BlueprintCallable, Category = "Character System|Inventory")
	void SetItemAt(int Index, class UInventoryItem* Item);

	UFUNCTION(BLueprintPure, Category = "Character System|Inventory")
	TArray<int> FindSuitableItemsPositions(int64 Flags);

	UFUNCTION(BlueprintCallable, Category = "Character System|Inventory", Meta = (ExpandEnumAsExecs = Success))
	void AddItemByRawID(FName RawID, int Count, EInventoryAddItemByRawIDResult& Success);

	UFUNCTION(BlueprintCallable, Category = "Character System|Inventory", Meta = (ExpandEnumAsExecs = Success))
	void AddItem(class UInventoryItem* Item, EInventoryAddItemResult& Success);

	UPROPERTY(BlueprintAssignable, Category = "Character System|Inventory")
	FInventoryResizedDelegate OnInventoryResized;

	UPROPERTY(BlueprintAssignable, Category = "Character System|Inventory")
	FItemChangedDelegate OnItemChanged;

	UPROPERTY(EditAnywhere, Category = "Character System|Inventory")
	int DefaultSize;

	UFUNCTION(BlueprintPure, Category = "Character System|Inventory")
	bool IsSlotEmpty(int index);

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UFUNCTION()
	void ItemUpdated(UInventoryItem* Sender);

	UFUNCTION()
	void ItemSplited(UInventoryItem* Sender, UInventoryItem* AnotherPart);

	UFUNCTION()
	void ItemMerged(UInventoryItem* Sender, UInventoryItem* Into);

	UFUNCTION()
	void ItemOwnerChanged(UInventoryItem* Sender);

	UFUNCTION(CallInEditor)
	void UpdateItems();

	UPROPERTY(EditAnywhere, EditFixedSize, Instanced, meta = (AllowPrivateAccess = true))
	TArray<class UInventoryItem*> Items;

	UPROPERTY(EditAnywhere, EditFixedSize, meta = (AllowPrivateAccess = true))
	TArray<FName> DefaultItems;

	UPROPERTY()
	TArray<FName> CachedDefaultItems;

	void TakeItemPrivate(UInventoryItem* Internal, int Count, EInventoryTakeItemResult& Success, UInventoryItem*& TakenItem);
};
