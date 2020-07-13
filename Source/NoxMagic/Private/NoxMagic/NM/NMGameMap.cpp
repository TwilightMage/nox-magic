#include "NMGameMap.h"
#include "NMPlayerController.h"
#include "MapMarkerActor.h"
#include "MapMarkerComponent.h"
#include "MapMarkerStatic.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"

UMapMarkerActor* ANMGameMap::PlaceMarkerOnActor(AActor* Actor, bool UpdateRotation, EMapMarkerType MarkerType)
{
	UMapMarkerActor* NewMarker = NewObject<UMapMarkerActor>();

	NewMarker->UpdateRotation = UpdateRotation;
	NewMarker->MarkerType = MarkerType;
	NewMarker->Target = Actor;

	AddMarker(NewMarker);
	return NewMarker;
}

UMapMarkerComponent* ANMGameMap::PlaceMarkerOnComponent(USceneComponent* Component, bool UpdateRotation, EMapMarkerType MarkerType)
{
	UMapMarkerComponent* NewMarker = NewObject<UMapMarkerComponent>();

	NewMarker->UpdateRotation = UpdateRotation;
	NewMarker->MarkerType = MarkerType;
	NewMarker->Target = Component;

	AddMarker(NewMarker);
	return NewMarker;
}

UMapMarkerStatic* ANMGameMap::PlaceMarkerOnLocation(const FVector& Location, EMapMarkerType MarkerType)
{
	UMapMarkerStatic* NewMarker = NewObject<UMapMarkerStatic>();

	NewMarker->UpdateRotation = false;
	NewMarker->MarkerType = MarkerType;
	NewMarker->Location = Location;

	AddMarker(NewMarker);
	return NewMarker;
}

void ANMGameMap::AddMarker(UMapMarker* Marker)
{
	if (Markers.AddUnique(Marker) >= 0)
	{
		Marker->GameMap = this;
		OnMarkerAdded.Broadcast(Marker);
	}
}

void ANMGameMap::RemoveMarker(UMapMarker* Marker)
{
	if (Markers.Find(Marker))
	{
		Markers.Remove(Marker);
		OnMarkerRemoved.Broadcast(Marker);
	}
}

TArray<class UMapMarker*> ANMGameMap::GetMarkers()
{
	return Markers;
}

FVector ANMGameMap::GetMapFocusPoint()
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClassWithTag(this, AActor::StaticClass(), "Map Focus", Actors);
	if (Actors.Num() > 0)
	{
		return Actors[0]->GetActorLocation();
	}
	return FVector::ZeroVector;
}

void ANMGameMap::BeginPlay()
{
	Super::BeginPlay();

	if (auto PlayerController = Cast<ANMPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		PlayerController->OnPossessed.AddDynamic(this, &ANMGameMap::PlayerPawnChanged);
	}
}

void ANMGameMap::PlayerPawnChanged(APawn* Pawn)
{
	if (PlayerMarker && PlayerMarker->IsValidLowLevel())
	{
		RemoveMarker(PlayerMarker);
		PlayerMarker = nullptr;
	}

	if (Pawn && Pawn->IsValidLowLevel())
	{
		PlayerMarker = PlaceMarkerOnActor(Pawn, true, EMapMarkerType::Player);
	}

	PlaceMarkerOnLocation(FVector(2760.f, 2820.f, 8130.f), EMapMarkerType::WayPoint);
}
