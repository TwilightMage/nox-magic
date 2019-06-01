

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NoxMagicFunctions.h"
#include "SlateImageBrush.h"
#include "Engine/Texture2D.h"
#include "SaveSlotMeta.generated.h"

USTRUCT(BlueprintType)
struct FSaveMeta
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadonly)
		FString name;

	UPROPERTY(BlueprintReadonly)
		FDateTime time;

	UPROPERTY(BlueprintReadonly)
		UTexture2D* thumb;

	FSaveMeta()
	{
		name = "";
		time = FDateTime::MinValue();
		thumb = nullptr;
	}

	FSaveMeta(FString name, FDateTime time, UTexture2D* thumb) : FSaveMeta()
	{
		this->name = name;
		this->time = time;
		this->thumb = thumb;
	}
};

UCLASS()
class NOXMAGIC_API USaveSlotMeta : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY()
		FString name;

	UPROPERTY()
		FDateTime time;

	UPROPERTY()
		UTexture2D* thumb;

	UFUNCTION()
		static USaveSlotMeta* Load(FString path);

	UFUNCTION()
		static void Save(FString path, FDateTime inTime, UTexture2D* inThumb);

	UFUNCTION()
		FSaveMeta GetStruct();
	
};
