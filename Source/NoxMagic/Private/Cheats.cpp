#include "Cheats.h"
#include "NoxMagicFunctions.h"

void UCheats::GetUID()
{
	UE_LOG(LogConsoleResponse, Warning, TEXT("UID: %s"), *UNoxMagicFunctions::UID());
}