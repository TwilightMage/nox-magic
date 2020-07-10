#include "Interactive.h"
#include "NMCharacter.h"

UInteractive::UInteractive()
{
	Name = FText::FromString("Some object");
	Action = FText::FromString("Interact");
}

void UInteractive::SetName(FText Name)
{
	this->Name = Name;
	if (OnUpdateDisplay.IsBound())
	{
		OnUpdateDisplay.Broadcast();
	}
}

void UInteractive::SetAction(FText Action)
{
	this->Action = Action;
	if (OnUpdateDisplay.IsBound())
	{
		OnUpdateDisplay.Broadcast();
	}
}

void UInteractive::Interact(ANMCharacter* character)
{
	if (OnInteract.IsBound())
	{
		OnInteract.Broadcast(character);
	}
}

void UInteractive::ShowHighlight()
{
	if (OnHighlightStateChanged.IsBound())
	{
		OnHighlightStateChanged.Broadcast(true);
	}
}

void UInteractive::HideHighlight()
{
	if (OnHighlightStateChanged.IsBound())
	{
		OnHighlightStateChanged.Broadcast(false);
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
