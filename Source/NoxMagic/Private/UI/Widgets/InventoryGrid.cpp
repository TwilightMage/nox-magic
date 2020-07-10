#include "InventoryGrid.h"
#include "Inventory.h"
#include "InventorySlot.h"

#include "Components/GridPanel.h"
#include "Components/Image.h"
#include "Blueprint/WidgetTree.h"

void UInventoryGrid::NativePreConstruct()
{
	Super::NativePreConstruct();

	const int RowSize = 10;
	if (auto Grid = GetGrid())
	{
		for (int i = 0; i < RowSize; i++)
		{
			Grid->SetColumnFill(i, 1.f);
		}

		if (Inventory && Inventory->IsValidLowLevel())
		{
			if (SlotClass)
			{
				for (int i = 0; i < Inventory->GetSize(); i++)
				{
					if (auto NewSlot = WidgetTree->ConstructWidget<UInventorySlot>(SlotClass))
					{
						NewSlot->SetInventory(Inventory);
						NewSlot->SetResponsiveIndex(i);
						Grid->AddChildToGrid(NewSlot, i % RowSize, i / RowSize);
					}
				}
			}
		}
		else
		{
			for (int i = 0; i < 25; i++)
			{
				Grid->AddChildToGrid(WidgetTree->ConstructWidget<UImage>(), i % RowSize, i / RowSize);
			}
		}
	}
}
