#pragma once

#include "DefaultItem.generated.h"

USTRUCT(BlueprintType)
struct FDefaultItem
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadonly)
	FName RawID;

	UPROPERTY(EditAnywhere, BlueprintReadonly)
	int Count = 1;

	bool operator==(const FDefaultItem& rhs) const
	{
		return RawID == rhs.RawID && Count == rhs.Count;
	}
};