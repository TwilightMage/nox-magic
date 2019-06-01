#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DataContainer.generated.h"

UENUM(BlueprintType)
enum class EGetContainerBranch : uint8
{
	Success,
	Fail
};

UCLASS(Blueprintable, Category = "NoxMagic")
class NOXMAGIC_API UDataContainer : public UObject
{
	GENERATED_BODY()

private:
	static UDataContainer* container;
	
public:
	UFUNCTION(BlueprintCallable, Category = "NoxMagic|Data container", Meta = (DeterminesOutputType = "containerType"))
		static UDataContainer* InitializeContainer(TSubclassOf<UDataContainer> containerType);

	UFUNCTION(BlueprintCallable, Category = "NoxMagic|Data container", Meta = (ExpandEnumAsExecs = "branch", DeterminesOutputType = "containerType"))
		static UDataContainer* GetContainer(TSubclassOf<UDataContainer> containerType, EGetContainerBranch& branch);
};
