#pragma once

#include "CoreMinimal.h"
#include "Engine/SkeletalMesh.h"
#include "InventoryItem.h"
#include "Item.generated.h"

USTRUCT()
struct FItem
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere)
		FName id;

	UPROPERTY(EditAnywhere)
		EItemType itemType;

	UPROPERTY(EditAnywhere)
		FText Name;

	UPROPERTY(EditAnywhere)
		FText description;

	UPROPERTY(EditAnywhere)
		USkeletalMesh* Mesh;
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