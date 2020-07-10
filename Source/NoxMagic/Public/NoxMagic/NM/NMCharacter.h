#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NMCharacter.generated.h"

UCLASS(ClassGroup = (NoxMagic))
class NOXMAGIC_API ANMCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	class UInteractive* GetInteractive() const { return Interactive; }
	class UInventory* GetInventory() const { return Inventory; }

	ANMCharacter();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void PickUp(class ADrop* Drop);

private:
	UPROPERTY(Category = Interaction, VisibleAnywhere, meta = (ExposeFunctionCategories = Interaction, AllowPrivateAccess = true))
	class UInteractive* Interactive = nullptr;

	UPROPERTY(Category = Inventory, VisibleAnywhere, meta = (ExposeFunctionCategories = Interaction, AllowPrivateAccess = true))
	class UInventory* Inventory = nullptr;
};
