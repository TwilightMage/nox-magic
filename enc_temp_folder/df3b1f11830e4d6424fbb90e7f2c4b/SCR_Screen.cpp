#include "SCR_Screen.h"

void USCR_Screen::Close()
{
	OnClose();
	ScreenCloseEvent.Broadcast();
}
