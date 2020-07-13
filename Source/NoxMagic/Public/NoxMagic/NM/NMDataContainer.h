#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NMDataContainer.generated.h"

UENUM(BlueprintType)
enum class EGetContainerBranch : uint8
{
	Success,
	Fail
};

UCLASS(Blueprintable, Category = "NoxMagic")
class NOXMAGIC_API UNMDataContainer : public UObject
{
	GENERATED_BODY()

private:
	static UNMDataContainer* container;
	
public:
	UFUNCTION(BlueprintCallable, Category = "NoxMagic|Data container", Meta = (DeterminesOutputType = "containerType"))
		static UNMDataContainer* InitializeContainer(TSubclassOf<UNMDataContainer> containerType);

	UFUNCTION(BlueprintCallable, Category = "NoxMagic|Data container", Meta = (ExpandEnumAsExecs = "branch", DeterminesOutputType = "containerType"))
		static UNMDataContainer* GetContainer(TSubclassOf<UNMDataContainer> containerType, EGetContainerBranch& branch);

	UFUNCTION(BlueprintPure, Category = "NoxMagic|Data container")
		static bool DoesContainerExists(TSubclassOf<UNMDataContainer> containerType);
};
