#include "WGT_MapMarker.h"
#include "MapMarker.h"
#include "NMGameMap.h"
#include "Components/Image.h"
#include "GameFramework/Pawn.h"
#include "Components/SceneComponent.h"
#include "Components/CanvasPanelSlot.h"
#include "Engine/Texture2D.h"

void UWGT_MapMarker::SetMarker(UMapMarker* NewMarker)
{
	Marker = NewMarker;
	UTexture2D* Tex = *TypeIcons.Find(Marker->MarkerType);
	GetImage()->SetBrushFromTexture(Tex, true);
	
	if (Tex)
	{
		if (auto CanvasSlot = Cast<UCanvasPanelSlot>(Slot))
		{
			CanvasSlot->SetSize(FVector2D(Tex->GetSizeX(), Tex->GetSizeY()));
		}
	}
}

void UWGT_MapMarker::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	FVector2D Loc = FVector2D(Marker->GetLocation());
	FVector2D Foc = FVector2D(Marker->GetGameMap()->GetMapFocusPoint());
	float Scale = 4000.f / 10.f;
	GetImage()->SetRenderTranslation(150.f * (Foc - Loc) / 2000.f);
	if (Marker->UpdateRotation)
	{
		GetImage()->SetRenderTransformAngle(Marker->GetRotationAngle());
	}
}
