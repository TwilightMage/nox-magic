#pragma once

#include "CoreMinimal.h"
#include "NoxMagic/MapMarker.h"
#include "MapMarkerStatic.generated.h"

UCLASS()
class NOXMAGIC_API UMapMarkerStatic : public UMapMarker
{
	GENERATED_BODY()

public:
	virtual FVector GetLocation_Implementation() override;
	virtual float GetRotationAngle_Implementation() override;

	UPROPERTY(BLueprintReadOnly, Meta = (ExposeOnSpawn = True))
	FVector Location;
};
