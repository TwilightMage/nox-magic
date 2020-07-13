#include "Inventory.h"
#include "Kismet/GameplayStatics.h"
#include "InventoryItem.h"
#include "NMGameMode.h"

UInventory::UInventory()
{
	PrimaryComponentTick.bCanEverTick = true;
	bReplicates = true;
}

void UInventory::SetMaxWeight(float NewMaxWeight)
{
	MaxWeight = NewMaxWeight;

	OnMaxWeightChanged.Broadcast();
}

void UInventory::IncrementMaxWeight(float Delta)
{
	SetMaxWeight(MaxWeight + Delta);
}

TArray<class UInventoryItem*> UInventory::GetItems()
{
	return Items;
}

bool UInventory::CanRecieveItemByRawID(FName RawID, int Count)
{
	if (auto ItemDefaults = ANMGameMode::FetchItemDefaults(RawID))
	{
		return GetFreeWeight() - ItemDefaults->Weight * Count > 0;
	}
	return false;
}

bool UInventory::CanRecieveItem(UInventoryItem* Target)
{
	return Target && GetFreeWeight() >= Target->GetWeightTotal();
}

bool UInventory::HaveItemByRawID(FName RawID, int Count)
{
	int HaveCount = 0;

	for (auto Item : Items)
	{
		if (Item && Item->GetRawID() == RawID)
		{
			HaveCount += Item->GetCount();

			if (Item->GetCount() >= Count)
			{
				return true;
			}
		}
	}

	return false;
}

bool UInventory::HaveItemByPattern(UInventoryItem* Pattern, int Count)
{
	int haveCount = 0;

	for (auto item : Items)
	{
		if (item && item->IsCompatibleWith(Pattern))
		{
			haveCount += item->GetCount();

			if (haveCount >= Count) return true;
		}
	}

	return false;
}

void UInventory::TakeItemsByRawID(FName RawID, int Count, EInventoryTakeItemResult& Success, TArray<UInventoryItem*>& TakenItems)
{
	TMap<int, int> FoundPositions;
	for (int i = 0; i < Items.Num(); i++)
	{
		if (Items[i]->GetRawID() == RawID)
		{
			if (Items[i]->GetCount() <= Count)
			{
				FoundPositions.Add(i, Items[i]->GetCount());
				Count -= Items[i]->GetCount();
				
				if (Count == 0)
				{
					break;
				}
			}
			else
			{
				FoundPositions.Add(i, Count);
				Count = 0;

				break;
			}
		}
	}

	if (Count > 0)
	{
		Success = EInventoryTakeItemResult::NotEnough;
		TakenItems = TArray<UInventoryItem*>();
	}

	TakenItems = TArray<UInventoryItem*>();

	for (auto KVP : FoundPositions)
	{
		TakenItems.Add(Items[KVP.Key]->Split(KVP.Value));
	}

	Success = EInventoryTakeItemResult::Success;
}

void UInventory::TakeItemByRawID(FName RawID, int Count, EInventoryTakeItemResult& Success, UInventoryItem*& TakenItem)
{
	for (auto Item : Items)
	{
		if (Item && Item->GetCount() >= Count && Item->GetRawID() == RawID)
		{
			Success = EInventoryTakeItemResult::Success;
			TakenItem = Item->Split(Count);
			return;
		}
	}

	Success = EInventoryTakeItemResult::NotEnough;
	TakenItem = nullptr;
}

void UInventory::TakeItemByPattern(UInventoryItem* Pattern, int Count, EInventoryTakeItemResult& Success, UInventoryItem*& TakenItem)
{
	for (auto Item : Items)
	{
		if (Item && Item->GetCount() >= Count && Item->IsCompatibleWith(Pattern))
		{
			Success = EInventoryTakeItemResult::Success;
			TakenItem = Item->Split(Count);
			return;
		}
	}

	Success = EInventoryTakeItemResult::NotEnough;
	TakenItem = nullptr;
}

TArray<UInventoryItem*> UInventory::FindSuitableItems(int64 Flags)
{
	TArray<UInventoryItem*> Result;

	for (auto Item : Items)
	{
		if (Item && Item->GetDefaults().SuitableSlot & Flags)
		{
			Result.Add(Item);
		}
	}

	return Result;
}

void UInventory::AddItemByRawID(FName RawID, int Count, EInventoryAddItemByRawIDResult& Success)
{
	if (UInventoryItem* item = UInventoryItem::CreateItem(RawID, Count))
	{
		EInventoryAddItemResult Suc;
		AddItem(item, Suc);

		switch (Suc)
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

void UInventory::AddItem(class UInventoryItem* ItemToAdd, EInventoryAddItemResult& Success)
{
	if (!CanRecieveItem(ItemToAdd))
	{
		Success = EInventoryAddItemResult::NotEnoughSpace;
		return;
	}

	for (auto Item : Items)
	{
		if (Item->IsCompatibleWith(ItemToAdd))
		{
			Item->Absorb(ItemToAdd);

			Success = EInventoryAddItemResult::Success;
			return;
		}
	}

	AddItem_SERVER(ItemToAdd);
	Success = EInventoryAddItemResult::Success;
}

float UInventory::GetFreeWeight()
{
	float Counter = MaxWeight;
	for (auto Item : Items)
	{
		Counter -= Item->GetWeightTotal();
	}

	return Counter;
}

void UInventory::BeginPlay()
{
	Super::BeginPlay();

	UpdateItems();
}

void UInventory::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (auto& Item : Items)
	{
		if (Item) Item->Tick(DeltaTime);
	}
}

void UInventory::UpdateItems()
{
	Items.Remove(nullptr);

	TSet<FName> ItemsToAdd = DefaultItems;

	for (int i = 0; i < Items.Num(); i++)
	{
		if (ItemsToAdd.Contains(Items[i]->GetRawID()))
		{
			ItemsToAdd.Remove(Items[i]->GetRawID());
			Items[i]->RefreshDefaults();
		}
		else
		{
			Items.RemoveAt(i);
			i--;
		}
	}

	for (auto RawID : ItemsToAdd)
	{
		if (auto NewItem = UInventoryItem::CreateItem(RawID))
		{
			Items.Add(NewItem);
		}
	}
}

void UInventory::ItemCountChanged(UInventoryItem* Sender)
{
	if (!UInventoryItem::IsItemValid(Sender))
	{
		int Index;
		if (Items.Find(Sender, Index))
		{
			Items[Index]->SetNewOwner(GetTransientPackage());
		}
	}
	else
	{
		if (Sender->GetWeightTotal() > 0)
		{
			OnWeightChanged.Broadcast();
		}

		int Index;
		if (Items.Find(Sender, Index))
		{
			SyncItem_SERVER(Index, Sender);
		}
	}
}

void UInventory::ItemSplited(UInventoryItem* Sender, UInventoryItem* AnotherPart)
{
	if (!UInventoryItem::IsItemValid(Sender))
	{
		int Index;
		if (Items.Find(Sender, Index))
		{
			Items[Index]->SetNewOwner(GetTransientPackage());
		}
	}
	else
	{
		int Index;
		if (Items.Find(Sender, Index))
		{
			SyncItem_SERVER(Index, Sender);
		}
	}
}

void UInventory::ItemAbsorbed(UInventoryItem* Sender, UInventoryItem* Into)
{
	int Index;
	if (Items.Find(Sender, Index))
	{
		Items[Index]->SetNewOwner(GetTransientPackage());
	}
}

void UInventory::ItemOwnerChanged(UInventoryItem* Sender)
{
	int Index;
	if (Items.Find(Sender, Index))
	{
		RemoveItem_SERVER(Index);
	}
}

void UInventory::ItemWeightChanged(UInventoryItem* Sender)
{
	if (Sender->GetWeightTotal() > 0)
	{
		OnWeightChanged.Broadcast();
	}

	int Index;
	if (Items.Find(Sender, Index))
	{
		SyncItem_SERVER(Index, Sender);
	}
}

void UInventory::IncrementItem_SERVER_Implementation(int Index, int Count)
{
	IncrementItem_MULTICAST(Index, Count);
}

void UInventory::IncrementItem_MULTICAST_Implementation(int Index, int Count)
{
	Items[Index]->AddCount(Count);

	if (Items[Index]->GetWeightForOne() * Count > 0)
	{
		OnWeightChanged.Broadcast();
	}
}

void UInventory::AddItem_SERVER_Implementation(UInventoryItem* Item)
{
	AddItem_MULTICAST(Item);
}

void UInventory::AddItem_MULTICAST_Implementation(UInventoryItem* Item)
{
	Items.Add(Item);

	Item->SetNewOwner(this);

	Item->OnCountChanged.AddDynamic(this, &UInventory::ItemCountChanged);
	Item->OnSplited.AddDynamic(this, &UInventory::ItemSplited);
	Item->OnAbsorbed.AddDynamic(this, &UInventory::ItemAbsorbed);
	Item->OnOwnerChanged.AddDynamic(this, &UInventory::ItemOwnerChanged);
	Item->OnWeightChanged.AddDynamic(this, &UInventory::ItemWeightChanged);

	if (Item->GetWeightTotal() > 0)
	{
		OnWeightChanged.Broadcast();
	}

	OnItemAdded.Broadcast(Item);
}

void UInventory::RemoveItem_SERVER_Implementation(int Index)
{
	RemoveItem_MULTICAST(Index);
}

void UInventory::RemoveItem_MULTICAST_Implementation(int Index)
{
	UInventoryItem* Item = Items[Index];

	Items.Remove(Item);

	Item->OnCountChanged.RemoveAll(this);
	Item->OnSplited.RemoveAll(this);
	Item->OnAbsorbed.RemoveAll(this);
	Item->OnOwnerChanged.RemoveAll(this);
	Item->OnWeightChanged.RemoveAll(this);

	if (Item->GetWeightTotal())
	{
		OnWeightChanged.Broadcast();
	}

	OnItemRemoved.Broadcast(Item);
}

void UInventory::SyncItem_SERVER_Implementation(int Index, UInventoryItem* Item)
{
	SyncItem_MULTICAST(Index, Item);
}

void UInventory::SyncItem_MULTICAST_Implementation(int Index, UInventoryItem* Item)
{
	Items[Index]->SetCount(Item->GetCount());
	Items[Index]->SetAdditiionalWeight(Item->GetAdditiionalWeight());
}
