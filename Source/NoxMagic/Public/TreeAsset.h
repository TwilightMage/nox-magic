#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TreeNodeDynamic.h"
#include "TreeAsset.generated.h"

USTRUCT(BlueprintType)
struct FTreeNode
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D position;

	FTreeNode()
	{
		Name = "";
		position = FVector2D::ZeroVector;
	}

	FTreeNode(FName Name, FVector2D position)
	{
		this->Name = Name;
		this->position = position;
	}
};

UCLASS(BlueprintType, Category = "NoxMagic")
class NOXMAGIC_API UTreeAsset : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FTreeNode> nodes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<int, int> parents;

	UFUNCTION(BlueprintPure, Category = "NoxMagic|RuneTree")
		static UTreeNodeDynamic* FormTree(TArray<FTreeNode> inputNodes, TMap<int, int> inputParents);

	UFUNCTION(BlueprintPure, Category = "NoxMagic|RuneTree")
		static UTreeNodeDynamic* FormTreeFromAsset(UTreeAsset* asset);
};
