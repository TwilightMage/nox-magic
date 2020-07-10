#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SoundSlider.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSoundSliderValueChangedEvent, float, NewValue);

UCLASS()
class NOXMAGIC_API USoundSlider : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetDisplayValue(float NewValue);

	UPROPERTY(BlueprintAssignable)
	FSoundSliderValueChangedEvent SoundSliderValueChangedEvent;

protected:
	UFUNCTION(BlueprintCallable)
	void SubmitNewValue(float NewValue);
};
