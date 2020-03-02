#include "Interactive.h"
#include "NM_Character.h"

UInteractive::UInteractive()
{
	name = FText::FromString("Some object");
	action = FText::FromString("Interact");
}

void UInteractive::SetName(FText name)
{
	this->name = name;
	onUpdateDisplay.Broadcast();
}

void UInteractive::SetAction(FText action)
{
	this->action = action;
	onUpdateDisplay.Broadcast();
}

void UInteractive::Interact(ANM_Character* character)
{
	onInteract.Broadcast(character);
}

void UInteractive::ShowHighlight()
{
	onHighlight.Broadcast(true);
}

void UInteractive::HideHighlight()
{
	onHighlight.Broadcast(false);
}

void UInteractive::HightlightMeshes(bool state, TArray<UStaticMeshComponent*> staticMeshes, TArray<USkeletalMeshComponent*> skeletalMeshes)
{
	for (auto st : staticMeshes)
	{
		st->SetRenderCustomDepth(state);
	}

	for (auto sk : skeletalMeshes)
	{
		sk->SetRenderCustomDepth(state);
	}
}
