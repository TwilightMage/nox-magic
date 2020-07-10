#include "SoundSlider.h"
#include "Components/Slider.h"

void USoundSlider::SubmitNewValue(float NewValue)
{
	SoundSliderValueChangedEvent.Broadcast(NewValue);
}
