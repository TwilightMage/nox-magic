#include "Inventory.h"
#include "Kismet/GameplayStatics.h"
#include "InventoryItem.h"
#include "NMGameMode.h"

UInventory::UInventory()
{
	PrimaryComponentTick.bCanEverTick = true;
}

int UInventory::GetSize() const
{
	return Items.Num();
}

void UInventory::SetSize(int NewSize)
{
	if (NewSize < 0) NewSize = 0;

	int oldSize = Items.Num();

	Items.SetNum(NewSize);

	OnInventoryResized.Broadcast();
}

void UInventory::IncrementSize(int Delta)
{
	SetSize(Items.Num() + Delta);
}

UInventoryItem* UInventory::GetItemAt(int Index)
{
	return Items[Index];
}

TArray<class UInventoryItem*> UInventory::GetItems()
{
	return Items;
}

bool UInventory::CanRecieveItemByRawID(FName RawID, int Count)
{
	return CanRecieveItem(UInventoryItem::CreateItem(RawID, Count));
}

bool UInventory::CanRecieveItem(UInventoryItem* Target)
{
	if (!Target) return false;

	int toDistribute = Target->GetCount();

	for (int i = 0; i < Items.Num(); i++)
	{
		if (Items[i] && Items[i]->GetRawID() == Target->GetRawID())
		{
			toDistribute -= (Target->GetDefaults().MaxStack - Items[i]->GetCount());
			if (toDistribute <= 0) return true;
		}
	}

	if (toDistribute > 0)
	{
		int additionalSlotsNeed = ceil(toDistribute / Target->GetDefaults().MaxStack);

		for (int i = 0; i < Items.Num(); i++)
		{
			if (IsSlotEmpty(i)) additionalSlotsNeed--;
			if (additionalSlotsNeed <= 0) return true;
		}
	}

	return false;
}

bool UInventory::HaveItemByRawID(FName RawID, int Count)
{
	int haveCount = 0;

	for (auto item : Items)
	{
		if (item && item->GetRawID() == RawID)
		{
			haveCount += item->GetCount();

			if (haveCount >= Count) return true;
		}
	}

	return false;
}

bool UInventory::HaveItemByPattern(UInventoryItem* Pattern, int Count)
{
	int haveCount = 0;

	for (auto item : Items)
	{
		if (item && item->GetRawID() == Pattern->GetRawID() && item->IsCompatibleWith(Pattern))
		{
			haveCount += item->GetCount();

			if (haveCount >= Count) return true;
		}
	}

	return false;
}

void UInventory::TakeItemAt(int Index, int Count, EInventoryTakeItemResult& Success, UInventoryItem*& TakenItem)
{
	if (Items[Index]->GetCount() < Count)
	{
		Success = EInventoryTakeItemResult::NotEnough;
		return;
	}
	else if (Items[Index]->GetCount() > Count)
	{
		TakenItem = Items[Index]->Split(Count);
	}
	else
	{
		TakenItem = Items[Index];
		SetItemAt(Index, nullptr);
	}

	Success = EInventoryTakeItemResult::Success;
}

void UInventory::TakeItemByRawID(FName RawID, int Count, EInventoryTakeItemResult& Success, UInventoryItem*& TakenItem)
{
	UInventoryItem* pattern = nullptr;
	TMap<UInventoryItem*, int> compatibleGroups;
	for (auto item : Items)
	{
		if (item)
		{
			bool groupFound = false;
			bool patternFound = false;

			for (auto group : compatibleGroups)
			{
				if (group.Key->IsCompatibleWith(item))
				{
					group.Value += item->GetCount();
					groupFound = true;

					if (group.Value >= Count)
					{
						pattern = group.Key;
						patternFound = true;
					}

					break;
				}
			}

			if (patternFound) break;
			if (!groupFound)
			{
				if (item->GetRawID() == RawID && item->GetCount() >= Count)
				{
					pattern = item;
					break;
				}
				else
				{
					compatibleGroups.Add(item, item->GetCount());
				}
			}
		}
	}

	if (!pattern)
	{
		Success = EInventoryTakeItemResult::NotEnough;
		TakenItem = nullptr;
		return;
	}

	TakeItemPrivate(pattern, Count, Success, TakenItem);
}

void UInventory::TakeItemByPattern(UInventoryItem* Pattern, int Count, EInventoryTakeItemResult& Success, UInventoryItem*& TakenItem)
{
	if (!HaveItemByPattern(Pattern, Count))
	{
		Success = EInventoryTakeItemResult::NotEnough;
		TakenItem = nullptr;
		return;
	}

	TakeItemPrivate(Pattern, Count, Success, TakenItem);
}

void UInventory::SetItemAt(int Index, class UInventoryItem* Item)
{
	if (Items[Index] == Item) return;

	if (Items[Index])
	{
		Items[Index]->OnUpdated.RemoveAll(this);
		Items[Index]->OnSplited.RemoveAll(this);
		Items[Index]->OnMerged.RemoveAll(this);
		Items[Index]->OnOwnerChanged.RemoveAll(this);
	}

	Items[Index] = Item;

	if (Items[Index])
	{
		Items[Index]->SetNewOwner(this);

		Items[Index]->OnUpdated.AddDynamic(this, &UInventory::ItemUpdated);
		Items[Index]->OnSplited.AddDynamic(this, &UInventory::ItemSplited);
		Items[Index]->OnMerged.AddDynamic(this, &UInventory::ItemMerged);
		Items[Index]->OnOwnerChanged.AddDynamic(this, &UInventory::ItemOwnerChanged);
	}

	OnItemChanged.Broadcast(Index);
}

TArray<int> UInventory::FindSuitableItemsPositions(int64 Flags)
{
	TArray<int> result;

	for (int i = 0; i < Items.Num(); i++)
	{
		if (Items[i] && Items[i]->GetDefaults().SuitableSlot & Flags)
		{
			result.Add(i);
		}
	}

	return result;
}

void UInventory::AddItemByRawID(FName RawID, int Count, EInventoryAddItemByRawIDResult& Success)
{
	if (UInventoryItem* item = UInventoryItem::CreateItem(RawID, Count))
	{
		EInventoryAddItemResult suc;
		AddItem(item, suc);

		switch (suc)
		{
		case EInventoryAddItemResult::Success:
			Success = EInventoryAddItemByRawIDResult::Success;
			break;
		case EInventoryAddItemResult::NotEnoughSpace:
			Success = EInventoryAddItemByRawIDResult::NotEnoughSpace;
			break;
		}

		return;
	}

	Success = EInventoryAddItemByRawIDResult::WrongRawID;
}

void UInventory::AddItem(class UInventoryItem* Item, EInventoryAddItemResult& Success)
{
	if (!CanRecieveItem(Item))
	{
		Success = EInventoryAddItemResult::NotEnoughSpace;
		return;
	}

	for (int i = 0; i < Items.Num(); i++)
	{
		if (Items[i] && Items[i]->IsCompatibleWith(Item))
		{
			int canAddToSlot = Item->GetDefaults().MaxStack - Items[i]->GetCount();
			if (Item->GetCount() <= canAddToSlot)
			{
				Items[i]->Merge(Item);
			}
			else
			{
				Items[i]->AddCount(canAddToSlot);
				Item->AddCount(-canAddToSlot);
			}

			if (Item->GetCount() == 0)
			{
				Success = EInventoryAddItemResult::Success;
				return;
			}
		}
	}

	if (Item->GetCount() > 0)
	{
		for (int i = 0; i < Items.Num(); i++)
		{
			if (!Items[i])
			{
				if (Item->GetCount() <= Item->GetDefaults().MaxStack)
				{
					SetItemAt(i, Item);

					Success = EInventoryAddItemResult::Success;
					return;
				}
				else
				{
					SetItemAt(i, Item->Split(Item->GetDefaults().MaxStack));
				}
			}
		}
	}
}

bool UInventory::IsSlotEmpty(int index)
{
	return Items[index] == nullptr;
}

void UInventory::BeginPlay()
{
	Super::BeginPlay();

	UpdateItems();
}

void UInventory::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (auto& item : Items)
	{
		if (item) item->Tick(DeltaTime);
	}
}

void UInventory::ItemUpdated(UInventoryItem* Sender)
{
	if (!UInventoryItem::IsItemValid(Sender))
	{
		int index;
		if (Items.Find(Sender, index))
		{
			SetItemAt(index, nullptr);
		}
	}
}

void UInventory::ItemSplited(UInventoryItem* Sender, UInventoryItem* AnotherPart)
{
	if (!UInventoryItem::IsItemValid(Sender))
	{
		int index;
		if (Items.Find(Sender, index))
		{
			SetItemAt(index, nullptr);
		}
	}
}

void UInventory::ItemMerged(UInventoryItem* Sender, UInventoryItem* Into)
{
	int index;
	if (Items.Find(Sender, index))
	{
		SetItemAt(index, nullptr);
	}
}

void UInventory::ItemOwnerChanged(UInventoryItem * Sender)
{
	int index;
	if (Items.Find(Sender, index))
	{
		SetItemAt(index, nullptr);
	}
}

void UInventory::TakeItemPrivate(UInventoryItem* Pattern, int Count, EInventoryTakeItemResult& Success, UInventoryItem*& TakenItem)
{
	UInventoryItem* result = Pattern->Clone(0);

	for (int i = 0; i < Items.Num(); i++)
	{
		if (Items[i] && Items[i]->IsCompatibleWith(Pattern))
		{
			if (Items[i]->GetCount() < Count)
			{
				Count -= Items[i]->GetCount();
				result->Merge(Items[i]);
				Items[i] = nullptr;
			}
			else
			{
				result->Merge(Items[i]->Split(Count));
				Count = 0;
			}

			if (Count == 0)
			{
				Success = EInventoryTakeItemResult::Success;
				TakenItem = result;
				return;
			}
		}
	}
}

void UInventory::UpdateItems()
{
	Items.SetNum(DefaultSize);
	DefaultItems.SetNum(DefaultSize);
	CachedDefaultItems.SetNum(DefaultSize);

	for (int i = 0; i < Items.Num(); i++)
	{
		const FName& def = DefaultItems[i];
		if (def != NAME_None)
		{
			if (def != CachedDefaultItems[i])
			{
				CachedDefaultItems[i] = def;

				if (UInventoryItem* newItem = UInventoryItem::CreateItem(def))
				{
					SetItemAt(i, newItem);
				}
				else
				{
					SetItemAt(i, nullptr);
				}
			}
			else if (Items[i] && Items[i]->IsValidLowLevel())
			{
				if (auto newDefaults = ANMGameMode::FetchItemDefaults(def))
				{
					Items[i]->SetDefaults(*newDefaults);
				}
				else
				{
					SetItemAt(i, nullptr);
				}
			}
		}
		else
		{
			SetItemAt(i, nullptr);
		}
	}

	CachedDefaultItems = DefaultItems;
}
