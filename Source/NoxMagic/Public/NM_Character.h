

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NM_Character.generated.h"

class UInteractive;

UCLASS(ClassGroup = (NoxMagic))
class NOXMAGIC_API ANM_Character : public ACharacter
{
	GENERATED_BODY()

public:
	ANM_Character();

	UPROPERTY(Category = Interaction, VisibleAnywhere, meta = (ExposeFunctionCategories = Interaction, AllowPrivateAccess = true))
		UInteractive* interactive = nullptr;
};
