#include "NMPlayerController.h"
#include "NMHUD_Play.h"
#include "Components/InputComponent.h"
#include "NMCharacter.h"
#include "Interactive.h"
#include "NoxMagicFunctions.h"
#include "GameFramework/Pawn.h"

#define LOCTEXT_NAMESPACE "NMPlayerController"

void ANMPlayerController::SetPausedState(bool NewState)
{
	if (Paused == NewState) return;

	Paused = NewState;

	if (Paused)
	{
		if (auto HUD = Cast<ANMHUD_Play>(GetHUD()))
		{
			HUD->SwitchPauseScreen(true);
		}
	}
	else
	{
		if (auto HUD = Cast<ANMHUD_Play>(GetHUD()))
		{
			HUD->SwitchPauseScreen(false);
		}
	}

	ProcessInputAvailable();
}

void ANMPlayerController::Loot(UInventory* Inventory, bool Force)
{
	if (SetCharacterState(ECharacterState::Looting, Force))
	{
		if (auto HUD = Cast<ANMHUD_Play>(GetHUD()))
		{
			if (auto MyCharacter = Cast<ANMCharacter>(GetPawn()))
			{
				FFormatNamedArguments Fmt;
				Fmt.Add("Name", MyCharacter->GetInteractive()->GetTitle());
				HUD->OpenLootingScreen(true, FText::Format(LOCTEXT("Player Inventory Title", "{Name}'s inventory"), Fmt).ToUpper(), MyCharacter->GetInventory(), FText::FromString("Storage"), Inventory);
			}
		}
	}
}

void ANMPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("PauseAction", EInputEvent::IE_Pressed, this, &ANMPlayerController::PauseAction);
	InputComponent->BindAction("InventoryAction", EInputEvent::IE_Pressed, this, &ANMPlayerController::InventoryAction);
}

void ANMPlayerController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);

	OnPossessed.Broadcast(NewPawn);
}

bool ANMPlayerController::SetCharacterState(ECharacterState NewState, bool Force)
{
	if (CharacterState != ECharacterState::Play && NewState != ECharacterState::Play && CharacterState != NewState && !Force)
	{
		return false;
	}

	if (auto HUD = Cast<ANMHUD_Play>(GetHUD()))
	{
		switch (CharacterState)
		{
		case ECharacterState::Inventory:
			HUD->CloseInventoryScreen();
			break;

		case ECharacterState::Looting:
			HUD->CloseLootingScreen();
			break;

		case ECharacterState::Trade:
			//HUD->CloseTradeScreen();
			break;
		}
	}

	CharacterState = NewState;

	UE_LOG(LogTemp, Log, TEXT("%s player state changed to %s"), *GetName(), *UNoxMagicFunctions::EnumToString(nameof(ECharacterState), CharacterState));

	ProcessInputAvailable();

	return true;
}

void ANMPlayerController::ProcessInputAvailable()
{
	if (Paused || CharacterState != ECharacterState::Play)
	{
		if (auto MyPawn = GetPawn())
		{
			MyPawn->DisableInput(this);
		}

		bShowMouseCursor = true;
	}
	else
	{
		if (auto MyPawn = GetPawn())
		{
			MyPawn->EnableInput(this);
		}

		bShowMouseCursor = false;

		SetInputMode(FInputModeGameOnly());
	}
}

void ANMPlayerController::PauseAction()
{
	if (CharacterState == ECharacterState::Play)
	{
		if (Paused)
		{
			SetPausedState(false);
		}
		else
		{
			SetPausedState(true);
		}
	}
	else
	{
		SetCharacterState(ECharacterState::Play, true);
	}
}

void ANMPlayerController::InventoryAction()
{
	if (CharacterState == ECharacterState::Inventory)
	{
		SetCharacterState(ECharacterState::Play, true);
	}
	else
	{
		if (SetCharacterState(ECharacterState::Inventory, false))
		{
			if (auto HUD = Cast<ANMHUD_Play>(GetHUD()))
			{
				if (auto MyCharacter = Cast<ANMCharacter>(GetPawn()))
				{
					FFormatNamedArguments Fmt;
					Fmt.Add("Name", MyCharacter->GetInteractive()->GetTitle());
					HUD->OpenInventoryScreen(true, FText::Format(LOCTEXT("Player Inventory Title", "{Name}'s inventory"), Fmt).ToUpper(), MyCharacter->GetInventory());
				}
			}
		}
	}
}
