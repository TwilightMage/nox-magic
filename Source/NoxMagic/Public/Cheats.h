

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "Cheats.generated.h"

/**
 * 
 */
UCLASS()
class NOXMAGIC_API UCheats : public UCheatManager
{
	GENERATED_BODY()

public:
	UFUNCTION(Exec)
		void GetUID();
};
