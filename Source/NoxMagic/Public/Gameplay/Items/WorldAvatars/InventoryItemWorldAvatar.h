#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InventoryItemWorldAvatar.generated.h"

UCLASS(Abstract, Blueprintable)
class NOXMAGIC_API AInventoryItemWorldAvatar : public AActor
{
	GENERATED_BODY()
	
public:	
	AInventoryItemWorldAvatar();
};
