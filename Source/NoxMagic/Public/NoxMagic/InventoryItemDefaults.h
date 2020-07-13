#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "InventoryItemDefaults.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Glove,
	Shoe,
	Pants,
	Vest,
	Hat,
	Ring,
	Necklace,
	MeleeWeapon,
	Staff,
	Bow,

	Max			UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FInventoryItemDefaults : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	FInventoryItemDefaults();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear)
	TSubclassOf<class UInventoryItem> InstanceClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AInventoryItemWorldAvatar> WorldAvatarClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (BitMask, BitMaskEnum = EItemType))
	int64 SuitableSlot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* Icon;
};