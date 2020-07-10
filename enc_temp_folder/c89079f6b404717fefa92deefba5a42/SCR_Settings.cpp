#include "SCR_Settings.h"
#include "SCR_SettingsScreen.h"
#include "Blueprint/WidgetTree.h"
#include "Components/NamedSlot.h"

void USCR_Settings::ShowScreen(ESettingsScreen NewScreen)
{
	if (CurrentScreenObject && CurrentScreenObject->IsValidLowLevel())
	{
		CurrentScreenObject->RemoveFromParent();
	}

	CurrentScreenObject = nullptr;

	if (auto SlotWidget = GetScreenSlot())
	{
		if (auto Class = ScreenClassMap.Find(NewScreen))
		{
			if (auto NewScreen = WidgetTree->ConstructWidget<USCR_SettingsScreen>(*Class))
			{
				SlotWidget->AddChild(NewScreen);
				CurrentScreenObject = NewScreen;
			}
		}
	}
}

void USCR_Settings::NativeConstruct()
{
	ShowScreen(ESettingsScreen::Sound);
}
