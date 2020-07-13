#include "WGT_Minimap.h"
#include "NMGameMode.h"
#include "NMGameMap.h"
#include "MapMarker.h"
#include "WGT_MapMarker.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CanvasPanel.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanelSlot.h"

void UWGT_Minimap::NativeConstruct()
{
	Super::NativeConstruct();

	if (ANMGameMode* GM = Cast<ANMGameMode>(UGameplayStatics::GetGameMode(GetOwningPlayer())))
	{
		if (ANMGameMap* GameMap = GM->GetGameMap())
		{
			Map = GameMap;
			Map->OnMarkerAdded.AddDynamic(this, &UWGT_Minimap::MapMarkerAdded);

			for (UMapMarker* Marker : Map->GetMarkers())
			{
				AddMarker(Marker);
			}
		}
	}
}

void UWGT_Minimap::MapMarkerAdded(UMapMarker* Marker)
{
	AddMarker(Marker);
}

void UWGT_Minimap::AddMarker(UMapMarker* Marker)
{
	if (MarkerClass.Get())
	{
		if (UCanvasPanel* Canvas = GetMarkersContainer())
		{
			if (UWGT_MapMarker* NewMarkerWidget = WidgetTree->ConstructWidget<UWGT_MapMarker>(MarkerClass))
			{
				if (auto CanvasSlot = Canvas->AddChildToCanvas(NewMarkerWidget))
				{
					NewMarkerWidget->SetMarker(Marker);

					CanvasSlot->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
					CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
					CanvasSlot->SetPosition(FVector2D(0.f, 0.f));
				}
			}
		}
	}
}
