#include "WGT_SoundSlider.h"
#include "Components/Slider.h"

void UWGT_SoundSlider::ChangeValue(float NewValue)
{
	OnValueChanged.Broadcast(NewValue);
}