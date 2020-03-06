

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayCharacter.generated.h"

class UInteractive;

UCLASS(ClassGroup = (NoxMagic))
class NOXMAGIC_API APlayCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayCharacter(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Category = Interaction, VisibleAnywhere, meta = (ExposeFunctionCategories = Interaction, AllowPrivateAccess = true))
		UInteractive* interactive = nullptr;
};
