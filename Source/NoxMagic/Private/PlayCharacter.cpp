#include "PlayCharacter.h"
#include "Interactive.h"

#define LOCTEXT_NAMESPACE "PlayCharacter"

APlayCharacter::APlayCharacter(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	interactive = ObjectInitializer.CreateDefaultSubobject<UInteractive>(this, FName("Interactive"));
	interactive->SetName(LOCTEXT("Default interaction action", "Pick"));
}
