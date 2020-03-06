#pragma once

#include "CoreMinimal.h"
#include "Engine/SkeletalMesh.h"
#include "Item.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Undefined,
	MeleeWeapon,
	Staff,
	Bow,
	Glove,
	Boot,
	Pants,
	Chestplate,
	Hat,
	Ring,
	Necklace
};

USTRUCT()
struct FItem
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere)
		FName id;

	UPROPERTY(EditAnywhere)
		EItemType itemType;

	UPROPERTY(EditAnywhere)
		FText name;

	UPROPERTY(EditAnywhere)
		FText description;

	UPROPERTY(EditAnywhere)
		USkeletalMesh* mesh;
};

USTRUCT()
struct FItemMeleeWeapon : public FItem
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere)
		float damage;
};

USTRUCT(BlueprintType)
struct FItemContainer
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY()
		FItem data;
};