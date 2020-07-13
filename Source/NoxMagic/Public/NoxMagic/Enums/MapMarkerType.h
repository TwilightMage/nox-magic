#pragma once

#include "MapMarkerType.generated.h"

UENUM(BlueprintType)
enum class EMapMarkerType : uint8
{
	Player,
	WayPoint,

	MAX		UMETA(Hidden)
};