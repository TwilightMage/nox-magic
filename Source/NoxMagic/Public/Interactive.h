#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Interactive.generated.h"

class ANM_Character;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUpdateDisplayEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHightlightEvent, bool, state);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractEvent, ANM_Character*, instigator);

UCLASS(ClassGroup = (NoxMagic), meta = (BlueprintSpawnableComponent))
class NOXMAGIC_API UInteractive : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractive();
	
	UFUNCTION(Category = Interaction, BlueprintSetter)
		void SetName(FText name);

	UFUNCTION(Category = Interaction, BlueprintSetter)
		void SetAction(FText action);

	UFUNCTION(Category = Interaction, BLueprintCallable)
		void Interact(ANM_Character* character);

	UFUNCTION(Category = Interaction, BlueprintCallable)
		void ShowHighlight();

	UFUNCTION(Category = Interaction, BlueprintCallable)
		void HideHighlight();

	UFUNCTION(Category = Interaction, BlueprintCallable)
		static void HightlightMeshes(bool state, TArray<UStaticMeshComponent*> staticMeshes, TArray<USkeletalMeshComponent*> skeletalMeshes);

	UPROPERTY(Category = Interaction, EditAnywhere, BlueprintSetter = SetName)
		FText name;

	UPROPERTY(Category = Interaction, EditAnywhere, BlueprintSetter = SetAction)
		FText action;

	UPROPERTY(Category = Interaction, BlueprintAssignable)
		FUpdateDisplayEvent onUpdateDisplay;

	UPROPERTY(Category = Interaction, BlueprintAssignable)
		FHightlightEvent onHighlight;

	UPROPERTY(Category = Interaction, BlueprintAssignable)
		FInteractEvent onInteract;
};
