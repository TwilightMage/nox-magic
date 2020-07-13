#include "WGT_InventoryItem.h"
#include "InventoryItem.h"
#include "Components/TextBlock.h"

#define LOCTEXT_NAMESPACE "Inventory Item Widget"

void UWGT_InventoryItem::Bind(class UInventoryItem* Item, FItemActions PossibleActions)
{
	for (auto KVP : GetActionButtons())
	{
		KVP.Value->SetVisibility(PossibleActions.Flags & ((int64)1 << (int64)KVP.Key) ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	//UImage* TypeIcon;

	if (Item)
	{
		if (auto NameText = GetNameText())
		{
			NameText->SetText(Item->GetDefaults().Name);
		}

		Item->OnCountChanged.AddDynamic(this, &UWGT_InventoryItem::ItemCountChanged);
		ItemCountChanged(Item);

		Item->OnWeightChanged.AddDynamic(this, &UWGT_InventoryItem::ItemWeightChanged);
		ItemWeightChanged(Item);

		ItemEquipedStateChanged(false);
	}
}

void UWGT_InventoryItem::ItemCountChanged(UInventoryItem* Sender)
{
	if (auto CountText = GetCountText())
	{
		FFormatNamedArguments Fmt;
		Fmt.Add("Count", Sender->GetCount());

		CountText->SetText(FText::Format(LOCTEXT("Count Format", "x{Count}"), Fmt));
		CountText->SetVisibility(Sender->GetCount() > 1 ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void UWGT_InventoryItem::ItemWeightChanged(UInventoryItem* Sender)
{
	if (auto WeightText = GetWeightText())
	{
		FFormatNamedArguments Fmt;
		Fmt.Add("Weight", FText::AsNumber(Sender->GetWeightTotal()));

		WeightText->SetText(FText::Format(LOCTEXT("Weight Format", "{Weight} kg"), Fmt));
	}
}

void UWGT_InventoryItem::ItemEquipedStateChanged(bool NewState)
{
	if (auto EquipedDisplay = GetEquipedDisplay())
	{
		EquipedDisplay->SetVisibility(NewState ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}
