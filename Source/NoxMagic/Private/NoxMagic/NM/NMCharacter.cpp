#include "NMCharacter.h"
#include "Interactive.h"
#include "Inventory.h"
#include "Drop.h"
#include "NMPlayerController.h"
#include "GameFramework/PlayerController.h"

#define LOCTEXT_NAMESPACE "NMCharacter"

ANMCharacter::ANMCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	InteractionDistance = 350;

	Interactive = CreateDefaultSubobject<UInteractive>(FName("Interactive"));
	Interactive->SetAction(LOCTEXT("Talk", "Talk"));

	Inventory = CreateDefaultSubobject<UInventory>(FName("Inventory"));
	Inventory->SetMaxWeight(50.f);
}

void ANMCharacter::SetCharacterName(FText NewCharacterName)
{
	CharacterName = NewCharacterName;
	Interactive->SetTitle(NewCharacterName);
}

void ANMCharacter::PickUp(ADrop* Drop)
{
	EInventoryAddItemResult AddItemResult;
	Inventory->AddItem(Drop->GetItem(), AddItemResult);
}

void ANMCharacter::Loot(class UInventory* TargetInventory)
{
	if (auto MyController = Cast<ANMPlayerController>(GetController()))
	{
		MyController->Loot(TargetInventory);
	}
}

void ANMCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (auto PlayerController = Cast<APlayerController>(GetController()))
	{
		UInteractive* SeeInteractive = nullptr;

		FHitResult OutHit;
		FVector TraceStart = PlayerController->PlayerCameraManager->GetCameraLocation();
		FVector TraceEnd = TraceStart + PlayerController->PlayerCameraManager->GetCameraRotation().RotateVector(FVector::ForwardVector) * InteractionDistance * 1.5f;
		auto ObjectTypes = FCollisionObjectQueryParams(
			ECC_TO_BITFIELD(ECC_WorldStatic) |
			ECC_TO_BITFIELD(ECC_WorldDynamic) |
			ECC_TO_BITFIELD(ECC_PhysicsBody) |
			ECC_TO_BITFIELD(ECC_GameTraceChannel1) // Drop
		);

		if (GetWorld()->LineTraceSingleByObjectType(OutHit, TraceStart, TraceEnd, ObjectTypes, FCollisionQueryParams("TraceInteractives", true, this)))
		{
			if (OutHit.bBlockingHit && OutHit.Actor != nullptr && OutHit.Actor->IsValidLowLevel())
			{
				auto AvailableInteractive = Cast<UInteractive>(OutHit.Actor->GetComponentByClass(UInteractive::StaticClass()));
				if (AvailableInteractive != nullptr && AvailableInteractive->IsValidLowLevel() && AvailableInteractive->IsActive())
				{
					if (GetDistanceTo(AvailableInteractive->GetOwner()) <= InteractionDistance)
					{
						SeeInteractive = AvailableInteractive;
					}
				}
			}
		}

		if (SeeInteractive)
		{
			if (SeeInteractive != FocusedInteractive)
			{
				if (FocusedInteractive != nullptr && FocusedInteractive->IsValidLowLevel())
				{
					FocusedInteractive->HideHighlight();
				}
				FocusedInteractive = SeeInteractive;
				FocusedInteractive->ShowHighlight();
			}
		}
		else
		{
			if (FocusedInteractive != nullptr && FocusedInteractive->IsValidLowLevel())
			{
				FocusedInteractive->HideHighlight();
				FocusedInteractive = nullptr;
			}
		}
	}
}

void ANMCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAction("InteractAction", EInputEvent::IE_Pressed, this, &ANMCharacter::InteractAction);
}

void ANMCharacter::InteractAction()
{
	if (FocusedInteractive && FocusedInteractive->IsValidLowLevel())
	{
		Interact_SERVER(FocusedInteractive);
	}
}

void ANMCharacter::Interact_SERVER_Implementation(UInteractive* Target)
{
	if (Target && Target->IsValidLowLevel())
	{
		Target->Interact(this);
	}
}