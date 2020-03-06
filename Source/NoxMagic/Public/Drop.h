#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/BillboardComponent.h"
#include "Engine/Classes/Materials/MaterialInstanceDynamic.h"
#include "Item.h"
#include "Drop.generated.h"

class UInteractive;
class APlayCharacter;

UCLASS(Blueprintable, ClassGroup = (NoxMagic))
class NOXMAGIC_API ADrop : public AActor
{
	GENERATED_BODY()
	
public:	
	ADrop();

#if WITH_EDITOR
	virtual void OnConstruction(const FTransform& Transform) override;
#endif

	UFUNCTION(BlueprintCallable, Category = NoxMagic, CallInEditor)
		void LoadContentFromDatabase();

	UFUNCTION(BlueprintCallable, NetMulticast, unreliable)
		void Disapear(FLinearColor color);

	UFUNCTION()
		void DisolveProcess(float progress);

	UFUNCTION()
		void DisolveFinished();

	UFUNCTION()
		void OnInteract(APlayCharacter* interactor);

	UFUNCTION()
		void OnHightLight(bool state);

	UPROPERTY()
		FItemContainer item;

	UPROPERTY(Category = Interaction, VisibleAnywhere, meta = (ExposeFunctionCategories = Interaction, AllowPrivateAccess = true))
		UInteractive* interactive;

	UPROPERTY(Category = Interaction, VisibleAnywhere, meta = (ExposeFunctionCategories = Interaction, AllowPrivateAccess = true))
		USkeletalMeshComponent* mesh;

	UPROPERTY()
		UTimelineComponent* DisolveTimeLine;

	UPROPERTY()
		UMaterialInstanceDynamic* mat;

	UPROPERTY()
		UBillboardComponent* billboard;

protected:
	virtual void BeginPlay() override;
};
