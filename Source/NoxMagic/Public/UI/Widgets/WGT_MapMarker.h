#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MapMarkerType.h"
#include "WGT_MapMarker.generated.h"

UCLASS()
class NOXMAGIC_API UWGT_MapMarker : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetMarker(class UMapMarker* NewMarker);

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	UFUNCTION(BlueprintPure, BlueprintImplementableEvent)
	class UImage* GetImage();

private:
	UPROPERTY()
	class UMapMarker* Marker;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Meta = (AllowPrivateAccess = True))
	TMap<EMapMarkerType, UTexture2D*> TypeIcons;
};
