#include "SCR_Screen.h"
#include "Components/NamedSlot.h"
#include "Blueprint/WidgetTree.h"
#include "Animation/UMGSequencePlayer.h"

void USCR_Screen::Close()
{
	CloseInternal();
	OnClose.Broadcast(this);
}

USCR_Screen* USCR_Screen::K2_OpenChildScreen(UNamedSlot* ParentSlot, TSubclassOf<USCR_Screen> Class)
{
	if (ParentSlot && ParentSlot->IsValidLowLevel())
	{
		if (Class.Get() && Class.Get()->IsValidLowLevel() && Class.Get() != USCR_Screen::StaticClass())
		{
			if (auto NewScreen = WidgetTree->ConstructWidget<USCR_Screen>(Class))
			{
				ParentSlot->AddChild(NewScreen);
				ChildrenScreens.Add(NewScreen);
				NewScreen->OnClose.AddDynamic(this, &USCR_Screen::ChildScreenClosed);

				return NewScreen;
			}
		}
	}

	return nullptr;
}

void USCR_Screen::NativeConstruct()
{
	Super::NativeConstruct();

	if (auto In = GetInAnim())
	{
		PlayAnimationForward(In);
	}
}

void USCR_Screen::ChildScreenClosed(USCR_Screen* Sender)
{
	ChildrenScreens.Remove(Sender);
}

void USCR_Screen::CloseInternal()
{
	K2_OnClose();
	while (ChildrenScreens.Num() > 0)
	{
		ChildrenScreens[0]->CloseInternal();
		ChildrenScreens.RemoveAt(0);
	}

	if (auto Out = GetOutAnim())
	{
		if (auto Instance = PlayAnimationForward(Out))
		{
			Instance->OnSequenceFinishedPlaying().AddLambda([&](UUMGSequencePlayer& Player)
			{
				RemoveFromParent();
			});
		}

		SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		RemoveFromParent();
	}
}
