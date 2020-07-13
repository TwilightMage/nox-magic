#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MapMarkerType.h"
#include "MapMarker.generated.h"

UCLASS(Abstract, Blueprintable)
class NOXMAGIC_API UMapMarker : public UObject
{
	GENERATED_BODY()

	friend class ANMGameMap;
	
public:
	class ANMGameMap* GetGameMap() const { return GameMap; }

	UFUNCTION(BLueprintCallable, BlueprintNativeEvent)
	FVector GetLocation();

	UFUNCTION(BLueprintCallable, BlueprintNativeEvent)
	float GetRotationAngle();

	UPROPERTY(BLueprintReadOnly, Meta = (ExposeOnSpawn = True))
	bool UpdateRotation;

	UPROPERTY(BLueprintReadOnly, Meta = (ExposeOnSpawn = True))
	EMapMarkerType MarkerType;

private:
	UPROPERTY()
	class ANMGameMap* GameMap;
};
