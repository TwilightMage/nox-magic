


#include "NM_Character.h"
#include "Interactive.h"

ANM_Character::ANM_Character()
{
	PrimaryActorTick.bCanEverTick = true;

	interactive = CreateDefaultSubobject<UInteractive>(FName("Interactive"));
	interactive->SetName(FText::FromString("Some player"));
}
