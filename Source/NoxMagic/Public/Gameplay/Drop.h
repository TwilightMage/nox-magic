#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/EngineTypes.h"
#include "Drop.generated.h"

UCLASS()
class NOXMAGIC_API ADrop : public AActor
{
	GENERATED_BODY()
	
public:	
	ADrop();

	class USkeletalMeshComponent* GetMesh() const { return Mesh; }
	class UBillboardComponent* GetBillboard() const { return Billboard; }
	class UInteractive* GetInteractable() const { return Interactive; }
	class UInventoryItem* GetItem() const { return Item; }

	UFUNCTION(BlueprintCallable, NetMulticast, unreliable)
	void Disapear(FLinearColor color);

	UPROPERTY(EditAnywhere)
	FName defaultRawID;

	UFUNCTION(BlueprintCallable, Category = Drop, Meta = (WorldContext = WorldContextObject))
	static ADrop* SpawnDropRaw(UObject* WorldContextObject, FTransform transform, FName rawID, int count);

	UFUNCTION(BLueprintCallable, Category = Drop, Meta = (WorldContext = WorldContextObject))
	static ADrop* SpawnDropPredefined(UObject* WorldContextObject, FTransform transform, class UInventoryItem* ContainedItem);

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	void Refreshed();

private:
	UFUNCTION()
	void InteractiveInteract(class ANMCharacter* interactor);

	UFUNCTION(Server, Reliable)
	void InteractiveInteract_SERVER(class ANMCharacter* interactor);

	UFUNCTION()
	void InteractiveHighlightStateChanged(bool newState);

	UFUNCTION()
	void DisolveFinish();

	UFUNCTION()
	void ItemCountChanged(UInventoryItem* Sender);

	UFUNCTION()
	void ItemSplited(UInventoryItem* Sender, UInventoryItem* AnotherPart);

	UFUNCTION()
	void ItemAbsorbed(UInventoryItem* Sender, UInventoryItem* Into);

	UFUNCTION()
	void ItemOwnerChanged(UInventoryItem* Sender);

	UPROPERTY(VisibleAnywhere, BLueprintReadOnly, Instanced, meta = (AllowPrivateAccess = true))
	class UInventoryItem* Item;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class USkeletalMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class UBillboardComponent* Billboard;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class UInteractive* Interactive;

	UPROPERTY(EditDefaultsOnly)
	class UParticleSystem* DisapearParticleSystem; // /Game/ParticleSystems/DropDisapear

	UPROPERTY(EditDefaultsOnly)
	class UMaterialInterface* DisapearMaterial; // /Game/Materials/Disolve/DisolveInstance

	UPROPERTY(EditDefaultsOnly)
	float DisolveDuration;

	UPROPERTY(EditDefaultsOnly)
	float DisolveParticlesLifetime;

	UPROPERTY()
	class UMaterialInstanceDynamic* Mat;

	UPROPERTY()
	FTimerHandle DisolveTimerHandle;
};
