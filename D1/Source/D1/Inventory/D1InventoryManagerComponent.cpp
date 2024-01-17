#include "D1InventoryManagerComponent.h"

#include "D1ItemInstance.h"
#include "Data/D1ItemData.h"
#include "Engine/ActorChannel.h"
#include "Fragments/D1ItemFragment.h"
#include "Fragments/D1ItemFragment_Stackable.h"
#include "Net/UnrealNetwork.h"
#include "System/D1AssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1InventoryManagerComponent)

FString FD1InventoryEntry::GetDebugString() const
{
	return FString::Printf(TEXT("[%s]"), *ItemInstance->GetDebugString());
}

bool FD1InventoryList::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
{
	return FFastArraySerializer::FastArrayDeltaSerialize<FD1InventoryEntry, FD1InventoryList>(Entries, DeltaParams, *this);
}

void FD1InventoryList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	if (OwnerComponent->OnInventoryEntryChanged.IsBound())
	{
		for (int32 Index : RemovedIndices)
		{
			const FD1InventoryEntry& Entry = Entries[Index];
			OwnerComponent->OnInventoryEntryChanged.Broadcast(Index, Entry.ItemInstance, 0);
		}
	}
}

void FD1InventoryList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	if (OwnerComponent->OnInventoryEntryChanged.IsBound())
	{
		for (int32 Index : AddedIndices)
		{
			const FD1InventoryEntry& Entry = Entries[Index];
			OwnerComponent->OnInventoryEntryChanged.Broadcast(Index, Entry.ItemInstance, Entry.ItemCount);
		}
	}
}

void FD1InventoryList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	if (OwnerComponent->OnInventoryEntryChanged.IsBound())
	{
		for (int32 Index : ChangedIndices)
		{
			const FD1InventoryEntry& Entry = Entries[Index];
			OwnerComponent->OnInventoryEntryChanged.Broadcast(Index, Entry.ItemInstance, Entry.ItemCount);
		}
	}
}

bool FD1InventoryList::TryAddItem(const FIntPoint& ItemPosition, UD1ItemInstance* ItemInstance, int32 ItemCount)
{
	// @TODO: Check Max Stack Count
	if (ItemInstance == nullptr || ItemCount <= 0)
		return false;
	
	check(OwnerComponent);
	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	check(ItemData);

	const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(ItemInstance->GetItemID());
	if (OwnerComponent->CanAddItemByPosition(ItemPosition, ItemDef.ItemSlotCount) == false)
		return false;

	OwnerComponent->MarkSlotChecks(true, ItemPosition, ItemDef.ItemSlotCount);

	const FIntPoint& InventorySlotCount = OwnerComponent->GetInventorySlotCount();
	FD1InventoryEntry& InventoryEntry = Entries[ItemPosition.Y * InventorySlotCount.X + ItemPosition.X];
	InventoryEntry.ItemInstance = ItemInstance;
	InventoryEntry.ItemCount = ItemCount;
				
	MarkItemDirty(InventoryEntry);
	return true;
}

bool FD1InventoryList::TryAddItem(int32 ItemID, int32 ItemCount)
{
	// @TODO: Check Max Stack Count
	if (ItemID <= 0 || ItemCount <= 0)
		return false;
	
	check(OwnerComponent);
	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	check(ItemData);

	const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(ItemID);
	if (ItemDef.FindFragmentByClass<UD1ItemFragment_Stackable>())
	{
		for (FD1InventoryEntry& Entry : Entries)
		{
			if (Entry.ItemInstance->GetItemID() == ItemID)
			{
				Entry.ItemCount += ItemCount;
				MarkItemDirty(Entry);
				return true;
			}
		}
	}
	
	const FIntPoint& ItemSlotCount = ItemDef.ItemSlotCount;
	TArray<TArray<bool>>& SlotChecks = OwnerComponent->GetSlotChecks();
	for (int32 y = 0; y <= SlotChecks.Num() - ItemSlotCount.Y; y++)
	{
		for (int32 x = 0; x <= SlotChecks[y].Num() - ItemSlotCount.X; x++)
		{
			if (SlotChecks[y][x])
				continue;

			FIntPoint ItemPosition = FIntPoint(x, y);
			if (OwnerComponent->CanAddItemByPosition(ItemPosition, ItemSlotCount))
			{
				OwnerComponent->MarkSlotChecks(true, ItemPosition, ItemSlotCount);

				const FIntPoint& InventorySlotCount = OwnerComponent->GetInventorySlotCount();
				FD1InventoryEntry& InventoryEntry = Entries[ItemPosition.Y * InventorySlotCount.X + ItemPosition.X];
				InventoryEntry.ItemInstance = NewObject<UD1ItemInstance>(OwnerComponent->GetOwner());
				InventoryEntry.ItemInstance->SetItemID(ItemID);
				InventoryEntry.ItemCount = ItemCount;

				for (const UD1ItemFragment* Fragment : ItemDef.Fragments)
				{
					if (Fragment)
					{
						Fragment->OnInstanceCreated(InventoryEntry.ItemInstance);
					}
				}
				
				MarkItemDirty(InventoryEntry);
				return true;
			}
		}
	}
	return false;
}

bool FD1InventoryList::TryAddItem(UD1ItemInstance* ItemInstance, int32 ItemCount)
{
	// @TODO: Check Max Stack Count
	if (ItemInstance == nullptr || ItemCount <= 0)
		return false;
	
	check(OwnerComponent);
	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	check(ItemData);

	const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(ItemInstance->GetItemID());
	if (ItemDef.FindFragmentByClass<UD1ItemFragment_Stackable>())
	{
		for (FD1InventoryEntry& Entry : Entries)
		{
			if (Entry.ItemInstance->GetItemID() == ItemInstance->GetItemID())
			{
				Entry.ItemCount += ItemCount;
				MarkItemDirty(Entry);
				return true;
			}
		}
	}
	
	const FIntPoint& ItemSlotCount = ItemDef.ItemSlotCount;
	TArray<TArray<bool>>& SlotChecks = OwnerComponent->GetSlotChecks();
	for (int32 y = 0; y <= SlotChecks.Num() - ItemSlotCount.Y; y++)
	{
		for (int32 x = 0; x <= SlotChecks[y].Num() - ItemSlotCount.X; x++)
		{
			if (SlotChecks[y][x])
				continue;

			FIntPoint ItemPosition = FIntPoint(x, y);
			if (OwnerComponent->CanAddItemByPosition(ItemPosition, ItemSlotCount))
			{
				OwnerComponent->MarkSlotChecks(true, ItemPosition, ItemSlotCount);
				
				const FIntPoint& InventorySlotCount = OwnerComponent->GetInventorySlotCount();
				FD1InventoryEntry& InventoryEntry = Entries[ItemPosition.Y * InventorySlotCount.X + ItemPosition.X];
				InventoryEntry.ItemInstance = ItemInstance;
				InventoryEntry.ItemCount = ItemCount;
				
				MarkItemDirty(InventoryEntry);
				return true;
			}
		}
	}
	return false;
}

bool FD1InventoryList::TryRemoveItem(const FIntPoint& ItemPosition, int32 ItemCount)
{
	// @TODO: Check Max Stack Count
	if (ItemCount <= 0)
		return false;

	check(OwnerComponent);
	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());
	
	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	check(ItemData);
	
	const FIntPoint& InventorySlotCount = OwnerComponent->GetInventorySlotCount();
	int32 Index = ItemPosition.Y * InventorySlotCount.X + ItemPosition.X;
	if (Entries.IsValidIndex(Index) == false)
		return false;
	
	FD1InventoryEntry& InventoryEntry = Entries[Index];
	if (InventoryEntry.ItemInstance == nullptr)
		return false;

	if (InventoryEntry.ItemCount < ItemCount)
		return false;

	InventoryEntry.ItemCount -= ItemCount;
	if (InventoryEntry.ItemCount == 0)
	{
		const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(InventoryEntry.ItemInstance->GetItemID());
		OwnerComponent->MarkSlotChecks(false, ItemPosition, ItemDef.ItemSlotCount);
		InventoryEntry.ItemInstance = nullptr;
	}
	MarkItemDirty(InventoryEntry);
	return true;
}

bool FD1InventoryList::TryRemoveItem(int32 ItemID, int32 ItemCount)
{
	// @TODO: Check Max Stack Count
	if (ItemID <= 0 || ItemCount <= 0)
		return false;

	check(OwnerComponent);
	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());
	
	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	check(ItemData);

	const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(ItemID);
	if (ItemDef.FindFragmentByClass<UD1ItemFragment_Stackable>() == nullptr)
		return false;

	if (ItemCount > GetTotalCountByID(ItemID))
		return false;
	
	for (int32 i = 0; i < Entries.Num(); i++)
	{
		FD1InventoryEntry& Entry = Entries[i];
		if (Entry.ItemInstance->GetItemID() == ItemID)
		{
			if (Entry.ItemCount > ItemCount)
			{
				Entry.ItemCount -= ItemCount;
				MarkItemDirty(Entry);
				break;
			}
			else
			{
				ItemCount -= Entry.ItemCount;
				
				const FIntPoint& InventorySlotCount = OwnerComponent->GetInventorySlotCount();
				FIntPoint ItemPosition = FIntPoint(i % InventorySlotCount.X, i / InventorySlotCount.X);
				OwnerComponent->MarkSlotChecks(false, ItemPosition, ItemDef.ItemSlotCount);

				Entry.ItemInstance = nullptr;
				Entry.ItemCount = 0;
				MarkItemDirty(Entry);
				
				if (ItemCount <= 0)
					break;
			}
		}
	}
	return true;
}

bool FD1InventoryList::TryIncreaseSlotCount(int32 NewCount)
{
	if (Entries.Num() >= NewCount)
		return false;

	Entries.SetNum(NewCount);
	for (FD1InventoryEntry& Entry : Entries)
	{
		MarkItemDirty(Entry);
	}
	return true;
}

FD1InventoryEntry FD1InventoryList::GetItemByPosition(const FIntPoint& ItemPosition)
{
	const FIntPoint& InventorySlotCount = OwnerComponent->GetInventorySlotCount();
	int32 Index = ItemPosition.Y * InventorySlotCount.X + ItemPosition.X;
	if (Entries.IsValidIndex(Index))
	{
		return Entries[Index];
	}
	return FD1InventoryEntry();
}

int32 FD1InventoryList::GetTotalCountByID(int32 ItemID)
{
	int32 Result = 0;
	for (FD1InventoryEntry& Entry : Entries)
	{
		if (Entry.ItemInstance->GetItemID() == ItemID)
		{
			Result += Entry.ItemCount;
		}
	}
	return Result;
}

UD1InventoryManagerComponent::UD1InventoryManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, InventoryList(this)
{
    SetIsReplicatedByDefault(true);
}

void UD1InventoryManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	TryIncreaseSlotCount(InventorySlotCount);
}

void UD1InventoryManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, InventoryList);
	DOREPLIFETIME(ThisClass, InventorySlotCount);
}

bool UD1InventoryManagerComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	for (FD1InventoryEntry& Entry : InventoryList.Entries)
	{
		UD1ItemInstance* Instance = Entry.ItemInstance;
		if (Instance && IsValid(Instance))
		{
			bWroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}
	return bWroteSomething;
}

bool UD1InventoryManagerComponent::TryIncreaseSlotCount(const FIntPoint& NewSlotCount)
{
	bool bResult = InventoryList.TryIncreaseSlotCount(NewSlotCount.X * NewSlotCount.Y);
	if (bResult)
	{
		InventorySlotCount = NewSlotCount;
	
		InventorySlotChecks.SetNumZeroed(InventorySlotCount.Y);
		for (int32 y = 0; y < InventorySlotChecks.Num(); y++)
		{
			InventorySlotChecks[y].SetNumZeroed(InventorySlotCount.X);
		}
	}
	return bResult;
}

bool UD1InventoryManagerComponent::TryAddItemByPosition(const FIntPoint& ItemPosition, UD1ItemInstance* ItemInstance, int32 ItemCount)
{	
	return InventoryList.TryAddItem(ItemPosition, ItemInstance, ItemCount);
}

bool UD1InventoryManagerComponent::TryAddItemByID(int32 ItemID, int32 ItemCount)
{
	return InventoryList.TryAddItem(ItemID, ItemCount);
}

bool UD1InventoryManagerComponent::TryAddItemByInstance(UD1ItemInstance* ItemInstance, int32 ItemCount)
{
	return InventoryList.TryAddItem(ItemInstance, ItemCount);
}

bool UD1InventoryManagerComponent::TryRemoveItemByPosition(const FIntPoint& ItemPosition, int32 ItemCount)
{
	return InventoryList.TryRemoveItem(ItemPosition, ItemCount);
}

bool UD1InventoryManagerComponent::TryRemoveItemByID(int32 ItemID, int32 ItemCount)
{
	return InventoryList.TryRemoveItem(ItemID, ItemCount);
}

const TArray<FD1InventoryEntry>& UD1InventoryManagerComponent::GetAllItems() const
{
	return InventoryList.GetAllItems();
}

FD1InventoryEntry UD1InventoryManagerComponent::GetItemByPosition(const FIntPoint& ItemPosition)
{
	return InventoryList.GetItemByPosition(ItemPosition);
}

bool UD1InventoryManagerComponent::CanAddItemByPosition(const FIntPoint& ItemPosition, const FIntPoint& ItemSlotCount)
{
	if (ItemPosition.Y + ItemSlotCount.Y > InventorySlotCount.Y || ItemPosition.X + ItemSlotCount.X > InventorySlotCount.X)
		return false;
	
	for (int32 y = ItemPosition.Y; y < ItemPosition.Y + ItemSlotCount.Y; y++)
	{
		for (int32 x = ItemPosition.X; x < ItemPosition.X + ItemSlotCount.X; x++)
		{
			if (InventorySlotChecks[y][x])
				return false;
		}
	}
	return true;
}

void UD1InventoryManagerComponent::MarkSlotChecks(bool bIsUsing, const FIntPoint& ItemPosition, const FIntPoint& ItemSlotCount)
{
	if (ItemPosition.Y + ItemSlotCount.Y > InventorySlotCount.Y || ItemPosition.X + ItemSlotCount.X > InventorySlotCount.X)
		return;
	
	for (int32 y = ItemPosition.Y; y < ItemPosition.Y + ItemSlotCount.Y; y++)
	{
		for (int32 x = ItemPosition.X; x < ItemPosition.X + ItemSlotCount.X; x++)
		{
			InventorySlotChecks[y][x] = bIsUsing;
		}
	}
}
