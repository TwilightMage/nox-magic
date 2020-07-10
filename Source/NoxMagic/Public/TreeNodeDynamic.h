#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TreeNodeDynamic.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTree, Log, All);

UCLASS(BlueprintType, Category = "NoxMagic")
class NOXMAGIC_API UTreeNodeDynamic : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<UTreeNodeDynamic*> children;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTreeNodeDynamic* parent;

	UFUNCTION(BlueprintPure, Category = "NoxMagic|RuneTree")
		static void MatchTrees(UTreeNodeDynamic* source, UTreeNodeDynamic* pattern, TMap<FName, UTreeNodeDynamic*>& matchedTrees, TMap<FName, FName>& matchedRunes, bool& success);

	UFUNCTION()
		bool IsTree();

	UFUNCTION()
		bool IsPlace();

	UFUNCTION()
		bool IsPattern();

	UFUNCTION()
		TArray<UTreeNodeDynamic*> RuneChildren();

	UFUNCTION()
		TArray<UTreeNodeDynamic*> PatternChildren();
};
