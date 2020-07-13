#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WGT_SoundSlider.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSoundSliderValueChangedEvent, float, NewValue);

UCLASS()
class NOXMAGIC_API UWGT_SoundSlider : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetDisplayValue(float NewValue);

	UPROPERTY(BlueprintAssignable)
	FSoundSliderValueChangedEvent OnValueChanged;

protected:
	UFUNCTION(BlueprintCallable)
	void ChangeValue(float NewValue);
};
