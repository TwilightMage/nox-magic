#include "MapMarkerActor.h"
#include "GameFramework/Actor.h"

FVector UMapMarkerActor::GetLocation_Implementation()
{
	if (Target && Target->IsValidLowLevel())
	{
		return Target->GetActorLocation();
	}
	
	return FVector::ZeroVector;
}

float UMapMarkerActor::GetRotationAngle_Implementation()
{
	if (Target && Target->IsValidLowLevel())
	{
		return Target->GetActorRotation().Yaw - 90.f;
	}

	return 0.f;
}
