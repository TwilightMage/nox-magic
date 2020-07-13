#include "Interactive.h"
#include "NMCharacter.h"

UInteractive::UInteractive()
{
	bAutoActivate = true;
	Title = FText::FromString("Some object");
	Action = FText::FromString("Interact");
}

void UInteractive::SetTitle(FText NewTitle)
{
	Title = NewTitle;
	OnUpdateDisplay.Broadcast();
}

void UInteractive::SetAction(FText Action)
{
	this->Action = Action;
	OnUpdateDisplay.Broadcast();
}

void UInteractive::Interact(ANMCharacter* character)
{
	OnInteract.Broadcast(character);
}

void UInteractive::ShowHighlight()
{
	OnHighlightStateChanged.Broadcast(true);
}

void UInteractive::HideHighlight()
{
	OnHighlightStateChanged.Broadcast(false);
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
