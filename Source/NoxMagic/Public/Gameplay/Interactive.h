#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Interactive.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUpdateDisplayEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHightlightStateChangedEvent, bool, state);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractEvent, class ANMCharacter*, instigator);

UCLASS(ClassGroup = (NoxMagic), meta = (BlueprintSpawnableComponent))
class NOXMAGIC_API UInteractive : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractive();
	
	UFUNCTION(Category = Interaction, BlueprintSetter)
	void SetName(FText Name);

	UFUNCTION(Category = Interaction, BlueprintSetter)
	void SetAction(FText Action);

	UFUNCTION(Category = Interaction, BLueprintCallable)
	void Interact(class ANMCharacter* character);

	UFUNCTION(Category = Interaction, BlueprintCallable)
	void ShowHighlight();

	UFUNCTION(Category = Interaction, BlueprintCallable)
	void HideHighlight();

	UFUNCTION(Category = Interaction, BlueprintCallable)
	static void HightlightMeshes(bool state, TArray<UStaticMeshComponent*> staticMeshes, TArray<USkeletalMeshComponent*> skeletalMeshes);

	UPROPERTY(Category = Interaction, EditAnywhere, BlueprintSetter = SetName)
	FText Name;

	UPROPERTY(Category = Interaction, EditAnywhere, BlueprintSetter = SetAction)
	FText Action;

	UPROPERTY(Category = Interaction, BlueprintAssignable)
	FUpdateDisplayEvent OnUpdateDisplay;

	UPROPERTY(Category = Interaction, BlueprintAssignable)
	FHightlightStateChangedEvent OnHighlightStateChanged;

	UPROPERTY(Category = Interaction, BlueprintAssignable)
	FInteractEvent OnInteract;
};
