#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WGT_Minimap.generated.h"

UCLASS()
class NOXMAGIC_API UWGT_Minimap : public UUserWidget
{
	GENERATED_BODY()
	
public:
	class ANMGameMap* GetMap() const { return Map; }

protected:
	UFUNCTION(BlueprintImplementableEvent)
	class UCanvasPanel* GetMarkersContainer();

	virtual void NativeConstruct() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Classes")
	TSubclassOf<class UWGT_MapMarker> MarkerClass;

	UFUNCTION()
	void MapMarkerAdded(class UMapMarker* Marker);

	UPROPERTY()
	class ANMGameMap* Map;

	void AddMarker(class UMapMarker* Marker);
};
