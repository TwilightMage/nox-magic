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

	UPROPERTY(Category = Interaction, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float InteractionDistance;

	UFUNCTION(BLueprintSetter)
	void SetCharacterName(FText NewCharacterName);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void PickUp(class ADrop* Drop);

	UFUNCTION(BlueprintCallable)
	void Loot(class UInventory* TargetInventory);

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(Category = Interaction, BlueprintReadOnly, VisibleAnywhere, meta = (ExposeFunctionCategories = Interaction, AllowPrivateAccess = true))
	class UInteractive* Interactive = nullptr;

	UPROPERTY(Category = Inventory, BlueprintReadOnly, VisibleAnywhere, meta = (ExposeFunctionCategories = Interaction, AllowPrivateAccess = true))
	class UInventory* Inventory = nullptr;

	UPROPERTY(Category = Interaction, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class UInteractive* FocusedInteractive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintSetter = SetCharacterName, Meta = (AllowPrivateAccess = True))
	FText CharacterName;

	UFUNCTION()
	void InteractAction();

	UFUNCTION(Server, Reliable)
	void Interact_SERVER(class UInteractive* Target);
};
