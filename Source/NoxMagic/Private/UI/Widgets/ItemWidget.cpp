#include "ItemWidget.h"
#include "InventoryItem.h"

void UItemWidget::DisplayItem(UInventoryItem* Item)
{
	if (Item && Item->IsValidLowLevel())
	{
		SetIcon(Item->GetDefaults().Icon);
		SetCount(Item->GetCount());
	}
	else
	{
		SetIcon(nullptr);
		SetCount(0);
	}
}

void UItemWidget::NativePreConstruct()
{
	SetIcon(nullptr);
	SetCount(0);
}
