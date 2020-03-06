#include "Interactive.h"
#include "PlayCharacter.h"

UInteractive::UInteractive()
{
	name = FText::FromString("Some object");
	action = FText::FromString("Interact");
}

void UInteractive::SetName(FText name)
{
	this->name = name;
	if (onUpdateDisplay.IsBound())
	{
		onUpdateDisplay.Broadcast();
	}
}

void UInteractive::SetAction(FText action)
{
	this->action = action;
	if (onUpdateDisplay.IsBound())
	{
		onUpdateDisplay.Broadcast();
	}
}

void UInteractive::Interact(APlayCharacter* character)
{
	if (onInteract.IsBound())
	{
		onInteract.Broadcast(character);
	}
}

void UInteractive::ShowHighlight()
{
	if (onHighlight.IsBound())
	{
		onHighlight.Broadcast(true);
	}
}

void UInteractive::HideHighlight()
{
	if (onHighlight.IsBound())
	{
		onHighlight.Broadcast(false);
	}
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
