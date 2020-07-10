

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
		FString Name;

	UPROPERTY(BlueprintReadonly)
		FDateTime time;

	UPROPERTY(BlueprintReadonly)
		UTexture2D* thumb;

	FSaveMeta()
	{
		Name = "";
		time = FDateTime::MinValue();
		thumb = nullptr;
	}

	FSaveMeta(FString Name, FDateTime time, UTexture2D* thumb)
	{
		this->Name = Name;
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
		FString Name;

	UPROPERTY()
		FDateTime time;

	UPROPERTY()
		FTextureRaw thumb;

	UFUNCTION()
		static USaveSlotMeta* Load(FString path);

	UFUNCTION()
		static USaveSlotMeta* Save(FString path, FDateTime inTime, UTexture2D* inThumb);

	UFUNCTION()
		FSaveMeta GetStruct();
	
};
