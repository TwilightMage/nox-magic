#include "SCR_Settings.h"
#include "SCR_SettingsScreen.h"

void USCR_Settings::ShowScreen(ESettingsScreen NewScreen)
{
	if (CurrentScreenObject && CurrentScreenObject->IsValidLowLevel())
	{
		CurrentScreenObject->Close();
	}

	CurrentScreenObject = nullptr;

	if (auto SlotWidget = GetScreenSlot())
	{
		if (auto Class = ScreenClassMap.Find(NewScreen))
		{
			if (auto NewScreen = OpenChildScreen<USCR_SettingsScreen>(SlotWidget, *Class))
			{
				CurrentScreenObject = NewScreen;
			}
		}
	}
}

void USCR_Settings::NativeConstruct()
{
	Super::NativeConstruct();

	ShowScreen(ESettingsScreen::Sound);
}
