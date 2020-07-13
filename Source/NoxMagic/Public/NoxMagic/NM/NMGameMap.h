#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapMarkerType.h"
#include "NMGameMap.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMapActorMarkerAdded, class UMapMarker*, Marker);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMapActorMarkerRemoved, class UMapMarker*, Marker);

UCLASS()
class NOXMAGIC_API ANMGameMap : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FMapActorMarkerAdded OnMarkerAdded;

	UPROPERTY(BlueprintAssignable)
	FMapActorMarkerRemoved OnMarkerRemoved;

	UFUNCTION(BlueprintCallable)
	class UMapMarkerActor* PlaceMarkerOnActor(class AActor* Actor, bool UpdateRotation, EMapMarkerType MarkerType);

	UFUNCTION(BlueprintCallable)
	class UMapMarkerComponent* PlaceMarkerOnComponent(class USceneComponent* Component, bool UpdateRotation, EMapMarkerType MarkerType);

	UFUNCTION(BlueprintCallable)
	class UMapMarkerStatic* PlaceMarkerOnLocation(const FVector& Location, EMapMarkerType MarkerType);

	UFUNCTION(BlueprintCallable)
	void AddMarker(class UMapMarker* Marker);

	UFUNCTION(BlueprintCallable)
	void RemoveMarker(class UMapMarker* Marker);

	UFUNCTION(BlueprintPure)
	TArray<class UMapMarker*> GetMarkers();

	UFUNCTION(BlueprintCallable)
	FVector GetMapFocusPoint();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	UMapMarker* PlayerMarker;

	UFUNCTION()
	void PlayerPawnChanged(class APawn* Pawn);

	UPROPERTY()
	TArray<class UMapMarker*> Markers;
};
