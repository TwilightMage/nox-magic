#include "NMHUD_Play.h"
#include "SCR_Screen.h"
#include "NMPlayerController.h"
#include "NMCharacter.h"
#include "SCR_Inventory.h"
#include "SCR_Looting.h"
#include "NoxMagicFunctions.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void ANMHUD_Play::SwitchPauseScreen(bool NewState)
{
	if (NewState)
	{
		if (auto NewScreen = OpenScreen(EScreenType::Pause))
		{
			UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(GetOwningPlayerController(), NewScreen, EMouseLockMode::LockAlways, false);

			NewScreen->OnClose.AddDynamic(this, &ANMHUD_Play::PauseScreenClosed);
		}
	}
	else
	{
		CloseScreen(EScreenType::Pause);
	}
}

void ANMHUD_Play::OpenInventoryScreen(bool ForceRefresh, FText Title, UInventory* Inventory)
{
	if (auto NewScreen = Cast<USCR_Inventory>(OpenScreen(EScreenType::Inventory, ForceRefresh)))
	{
		UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(GetOwningPlayerController(), NewScreen, EMouseLockMode::LockAlways, false);

		NewScreen->BindInventory(Title, Inventory);
	}
}

void ANMHUD_Play::CloseInventoryScreen()
{
	CloseScreen(EScreenType::Inventory);
}

void ANMHUD_Play::OpenLootingScreen(bool ForceRefresh, FText Title, UInventory* Inventory, FText AnotherTitle, UInventory* AnotherInventory)
{
	if (auto NewScreen = Cast<USCR_Looting>(OpenScreen(EScreenType::Looting, ForceRefresh)))
	{
		UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(GetOwningPlayerController(), NewScreen, EMouseLockMode::LockAlways, false);

		NewScreen->BindInventoryToInventory(Title, Inventory, AnotherTitle, AnotherInventory);
	}
}

void ANMHUD_Play::CloseLootingScreen()
{
	CloseScreen(EScreenType::Looting);
}

void ANMHUD_Play::BeginPlay()
{	
	Super::BeginPlay();

	OpenScreen(EScreenType::Play);
}

USCR_Screen* ANMHUD_Play::OpenScreen(EScreenType Screen, bool ForceRefresh)
{
	if (auto OldScreenInstance = ScreenInstances.Find(Screen))
	{
		if (ForceRefresh)
		{
			(*OldScreenInstance)->Close();
		}
		else
		{
			return nullptr;
		}
	}

	if (auto NewScreenDef = ScreenDefinitions.Find(Screen))
	{
		if (NewScreenDef->ScreenClass && NewScreenDef->ScreenClass.Get() && NewScreenDef->ScreenClass.Get()->IsValidLowLevel())
		{
			if (auto NewScreen = CreateWidget<USCR_Screen>(GetOwningPlayerController(), NewScreenDef->ScreenClass))
			{
				if (NewScreen->AddToPlayerScreen(NewScreenDef->Layer))
				{
					UE_LOG(LogTemp, Log, TEXT("Opened screen %s"), *UNoxMagicFunctions::EnumToString(nameof(EScreenType), Screen));

					ScreenInstances.Add(Screen, NewScreen);

					NewScreen->OnClose.AddDynamic(this, &ANMHUD_Play::ScreenClosed);

					return NewScreen;
				}
			}
		}
	}

	return nullptr;
}

void ANMHUD_Play::CloseScreen(EScreenType Screen)
{
	if (auto ScreenInstance = ScreenInstances.Find(Screen))
	{
		(*ScreenInstance)->Close();
	}
}

void ANMHUD_Play::PauseScreenClosed(USCR_Screen* Sender)
{
	if (auto PlayerController = Cast<ANMPlayerController>(GetOwningPlayerController()))
	{
		PlayerController->SetPausedState(false);
	}
}

void ANMHUD_Play::ScreenClosed(USCR_Screen* Sender)
{
	if (auto FoundKey = ScreenInstances.FindKey(Sender))
	{
		UE_LOG(LogTemp, Log, TEXT("Closed screen %s"), *UNoxMagicFunctions::EnumToString(nameof(EScreenType), *FoundKey));
		ScreenInstances.Remove(*FoundKey);
	}
}