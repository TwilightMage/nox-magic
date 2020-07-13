#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NMPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNMPlayerControllerPossessed, class APawn*, Pawn);

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	Play,
	Inventory,
	Looting,
	Trade,

	MAX		UMETA(Hidden)
};

UCLASS()
class NOXMAGIC_API ANMPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	bool IsPaused() const { return Paused; }

	UPROPERTY(BlueprintAssignable)
	FNMPlayerControllerPossessed OnPossessed;

	UFUNCTION(BlueprintCallable)
	void SetPausedState(bool NewState);

	UFUNCTION(BlueprintCallable)
	void Loot(class UInventory* Inventory, bool Force = false);

	virtual void SetupInputComponent() override;

protected:
	virtual void OnPossess(class APawn* NewPawn) override;

private:
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = True))
	bool Paused;

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = True))
	ECharacterState CharacterState;

	UFUNCTION(BlueprintCallable)
	bool SetCharacterState(ECharacterState NewState, bool Force);

	UFUNCTION()
	void ProcessInputAvailable();

	UFUNCTION()
	void PauseAction();

	UFUNCTION()
	void InventoryAction();
};
