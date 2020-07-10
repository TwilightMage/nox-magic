#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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
	void OnInteract(class ANMCharacter* interactor);

	UFUNCTION()
	void OnHighlightStateChanged(bool newState);

	UFUNCTION()
	void ItemUpdated(UInventoryItem* Sender);

	UFUNCTION()
	void ItemSplited(UInventoryItem* Sender, UInventoryItem* AnotherPart);

	UFUNCTION()
	void ItemMerged(UInventoryItem* Sender, UInventoryItem* Into);

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

	UPROPERTY(EditDefaultsOnly, Category = "Defaults")
	UParticleSystem* DisapearParticleSystem; // /Game/ParticleSystems/DropDisapear

	UPROPERTY(EditDefaultsOnly, Category = "Defaults")
	UMaterialInterface* DisapearMaterial; // /Game/Materials/Disolve/DisolveInstance

	UPROPERTY()
	FName CachedDefaultRawID;

	UPROPERTY()
	class UMaterialInstanceDynamic* Mat;

	UPROPERTY()
	class UTimelineComponent* DisolveTimeLine;
};
