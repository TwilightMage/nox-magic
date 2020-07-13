#include "MapMarkerComponent.h"
#include "Components/SceneComponent.h"

FVector UMapMarkerComponent::GetLocation_Implementation()
{
	if (Target && Target->IsValidLowLevel())
	{
		return Target->GetComponentLocation();
	}

	return FVector::ZeroVector;
}

float UMapMarkerComponent::GetRotationAngle_Implementation()
{
	if (Target && Target->IsValidLowLevel())
	{
		return Target->GetComponentRotation().Yaw - 90.f;
	}

	return 0.f;
}
