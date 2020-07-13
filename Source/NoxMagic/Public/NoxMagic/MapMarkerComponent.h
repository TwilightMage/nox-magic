#pragma once

#include "CoreMinimal.h"
#include "NoxMagic/MapMarker.h"
#include "MapMarkerComponent.generated.h"

UCLASS()
class NOXMAGIC_API UMapMarkerComponent : public UMapMarker
{
	GENERATED_BODY()
	
public:
	virtual FVector GetLocation_Implementation() override;
	virtual float GetRotationAngle_Implementation() override;

	UPROPERTY(BLueprintReadOnly, Meta = (ExposeOnSpawn = True))
	class USceneComponent* Target;
};
