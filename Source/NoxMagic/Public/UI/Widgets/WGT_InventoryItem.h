#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WGT_InventoryItem.generated.h"

UENUM(BlueprintType)
enum class EItemActions : uint8
{
	None		= 0,
	Use			= 1,
	Drop		= 2,
	Transfer	= 4,
	Sell		= 8,

	MAX		UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FItemActions
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Meta = (BitMask, BitMaskEnum = EItemActions))
	int64 Flags;
};

UCLASS()
class NOXMAGIC_API UWGT_InventoryItem : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	TMap<EItemActions, class UWidget*> GetActionButtons();

	UFUNCTION(BlueprintImplementableEvent)
	class UImage* GetTypeIcon();

	UFUNCTION(BlueprintImplementableEvent)
	class UTextBlock* GetNameText();

	UFUNCTION(BlueprintImplementableEvent)
	class UTextBlock* GetCountText();

	UFUNCTION(BlueprintImplementableEvent)
	class UTextBlock* GetWeightText();

	UFUNCTION(BlueprintImplementableEvent)
	class UWidget* GetEquipedDisplay();

	UFUNCTION(BLueprintCallable)
	void Bind(class UInventoryItem* Item, FItemActions PossibleActions);

private:
	UFUNCTION()
	void ItemCountChanged(class UInventoryItem* Sender);

	UFUNCTION()
	void ItemWeightChanged(class UInventoryItem* Sender);

	UFUNCTION()
	void ItemEquipedStateChanged(bool NewState);
};
