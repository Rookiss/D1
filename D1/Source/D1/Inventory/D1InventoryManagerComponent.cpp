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

void FD1InventoryList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	check(ItemData);

	const FIntPoint& InventorySlotCount = OwnerComponent->GetInventorySlotCount();
	
	for (int32 Index : AddedIndices)
	{
		const FD1InventoryEntry& Entry = Entries[Index];
		if (Entry.GetItemInstance() == nullptr)
			continue;

		const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID((Entry.GetItemInstance()->GetItemID()));
		const FIntPoint ItemPosition = FIntPoint(Index % InventorySlotCount.X, Index / InventorySlotCount.X);
		const FIntPoint& ItemSlotCount = ItemDef.ItemSlotCount;
		OwnerComponent->MarkSlotChecks(true, ItemPosition, ItemSlotCount);
		
		if (OwnerComponent->OnInventoryEntryChanged.IsBound())
		{
			OwnerComponent->OnInventoryEntryChanged.Broadcast(ItemPosition, Entry.ItemInstance, Entry.ItemCount);
		}
	}
}

void FD1InventoryList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	check(ItemData);

	const FIntPoint& InventorySlotCount = OwnerComponent->GetInventorySlotCount();
	
	TArray<int32> AliveIndices;
	AliveIndices.Reserve(FinalSize);
		
	for (int32 Index : ChangedIndices)
	{
		const FD1InventoryEntry& Entry = Entries[Index];
		if (Entry.ItemCount > 0)
		{
			AliveIndices.Add(Index);
			continue;
		}

		const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(Entry.CachedItemID);
		const FIntPoint ItemPosition = FIntPoint(Index % InventorySlotCount.X, Index / InventorySlotCount.X);
		const FIntPoint& ItemSlotCount = ItemDef.ItemSlotCount;
		OwnerComponent->MarkSlotChecks(false, ItemPosition, ItemSlotCount);

		if (OwnerComponent->OnInventoryEntryChanged.IsBound())
		{
			OwnerComponent->OnInventoryEntryChanged.Broadcast(ItemPosition, Entry.ItemInstance, Entry.ItemCount);
		}
	}

	for (int32 Index : AliveIndices)
	{
		const FD1InventoryEntry& Entry = Entries[Index];
		
		const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID((Entry.GetItemInstance()->GetItemID()));
		const FIntPoint ItemPosition = FIntPoint(Index % InventorySlotCount.X, Index / InventorySlotCount.X);
		const FIntPoint& ItemSlotCount = ItemDef.ItemSlotCount;
		OwnerComponent->MarkSlotChecks(true, ItemPosition, ItemSlotCount);
		
		if (OwnerComponent->OnInventoryEntryChanged.IsBound())
		{
			OwnerComponent->OnInventoryEntryChanged.Broadcast(ItemPosition, Entry.ItemInstance, Entry.ItemCount);
		}
	}
}

bool FD1InventoryList::TryAddItem(const FIntPoint& ItemPosition, UD1ItemInstance* ItemInstance, int32 ItemCount)
{
	// @TODO: Check Max Stack Count
	if (ItemInstance == nullptr || ItemCount <= 0)
		return false;

	if (ItemPosition.X < 0 || ItemPosition.Y < 0)
		return false;

	const FIntPoint& InventorySlotCount = OwnerComponent->GetInventorySlotCount();
	if (ItemPosition.X >= InventorySlotCount.X || ItemPosition.Y >= InventorySlotCount.Y)
		return false;
	
	check(OwnerComponent);
	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	check(ItemData);
	
	int32 Index = ItemPosition.Y * InventorySlotCount.X + ItemPosition.X;
	if (Entries.IsValidIndex(Index) == false)
		return false;

	const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(ItemInstance->GetItemID());
	FD1InventoryEntry& InventoryEntry = Entries[Index];
	if (InventoryEntry.ItemInstance)
	{
		if (InventoryEntry.ItemInstance->ItemID == ItemInstance->ItemID && ItemDef.FindFragmentByClass<UD1ItemFragment_Stackable>())
		{
			InventoryEntry.ItemCount += ItemCount;
			MarkItemDirty(InventoryEntry);
			return true;
		}
		return false;
	}
	else
	{
		if (OwnerComponent->CanAddItemByPosition(ItemPosition, ItemDef.ItemSlotCount) == false)
			return false;

		OwnerComponent->MarkSlotChecks(true, ItemPosition, ItemDef.ItemSlotCount);
	
		InventoryEntry.ItemInstance = ItemInstance;
		InventoryEntry.ItemCount = ItemCount;
		InventoryEntry.CachedItemID = ItemInstance->GetItemID();
				
		MarkItemDirty(InventoryEntry);
		return true;
	}
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
			if (Entry.ItemInstance == nullptr)
				continue;
			
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
				InventoryEntry.CachedItemID = ItemID;

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
			if (Entry.ItemInstance == nullptr)
				continue;
			
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
				InventoryEntry.CachedItemID = ItemInstance->GetItemID();
				
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

	if (ItemPosition.X < 0 || ItemPosition.Y < 0)
		return false;

	const FIntPoint& InventorySlotCount = OwnerComponent->GetInventorySlotCount();
	if (ItemPosition.X >= InventorySlotCount.X || ItemPosition.Y >= InventorySlotCount.Y)
		return false;
	
	check(OwnerComponent);
	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());
	
	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	check(ItemData);
	
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
		if (Entry.ItemInstance == nullptr)
			continue;
		
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

FD1InventoryEntry FD1InventoryList::GetItemByPosition(const FIntPoint& ItemPosition)
{
	if (ItemPosition.X < 0 || ItemPosition.Y < 0)
		return FD1InventoryEntry();

	const FIntPoint& InventorySlotCount = OwnerComponent->GetInventorySlotCount();
	if (ItemPosition.X >= InventorySlotCount.X || ItemPosition.Y >= InventorySlotCount.Y)
		return FD1InventoryEntry();
	
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

	InventorySlotChecks.SetNumZeroed(InventorySlotCount.Y);
	for (int32 y = 0; y < InventorySlotChecks.Num(); y++)
	{
		InventorySlotChecks[y].SetNumZeroed(InventorySlotCount.X);
	}
}

void UD1InventoryManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, InventoryList);
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

void UD1InventoryManagerComponent::Init()
{
	if (GetOwner()->HasAuthority() == false)
		return;
	
	InventoryList.Entries.SetNum(InventorySlotCount.X * InventorySlotCount.Y);
}

bool UD1InventoryManagerComponent::TryAddItemByPosition(const FIntPoint& ItemPosition, UD1ItemInstance* ItemInstance, int32 ItemCount)
{
	if (GetOwner()->HasAuthority() == false)
		return false;
	
	return InventoryList.TryAddItem(ItemPosition, ItemInstance, ItemCount);
}

bool UD1InventoryManagerComponent::TryAddItemByID(int32 ItemID, int32 ItemCount)
{
	if (GetOwner()->HasAuthority() == false)
		return false;
	
	return InventoryList.TryAddItem(ItemID, ItemCount);
}

bool UD1InventoryManagerComponent::TryAddItemByInstance(UD1ItemInstance* ItemInstance, int32 ItemCount)
{
	if (GetOwner()->HasAuthority() == false)
		return false;
	
	return InventoryList.TryAddItem(ItemInstance, ItemCount);
}

bool UD1InventoryManagerComponent::TryRemoveItemByPosition(const FIntPoint& ItemPosition, int32 ItemCount)
{
	if (GetOwner()->HasAuthority() == false)
		return false;
	
	return InventoryList.TryRemoveItem(ItemPosition, ItemCount);
}

bool UD1InventoryManagerComponent::TryRemoveItemByID(int32 ItemID, int32 ItemCount)
{
	if (GetOwner()->HasAuthority() == false)
		return false;
	
	return InventoryList.TryRemoveItem(ItemID, ItemCount);
}

void UD1InventoryManagerComponent::RequestMoveItem_Implementation(const FIntPoint& FromPosition, const FIntPoint& ToPosition)
{
	if (GetOwner()->HasAuthority() == false)
		return;
	
	if (FromPosition == ToPosition)
		return;

	if (FromPosition.X < 0 || FromPosition.Y < 0 || ToPosition.X < 0 || ToPosition.Y < 0)
		return;

	if (FromPosition.X >= InventorySlotCount.X || FromPosition.Y >= InventorySlotCount.Y ||
		ToPosition.X >= InventorySlotCount.X || ToPosition.Y >= InventorySlotCount.Y)
		return;
	
	const FD1InventoryEntry FromEntry = InventoryList.GetItemByPosition(FromPosition);
	const FD1InventoryEntry ToEntry = InventoryList.GetItemByPosition(ToPosition);
	
	if (FromEntry.ItemInstance == nullptr || ToEntry.ItemInstance)
		return;

	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	check(ItemData);

	const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(FromEntry.ItemInstance->GetItemID());
	if (CanMoveItemByPosition(FromPosition, ToPosition, ItemDef.ItemSlotCount) == false)
		return;

	InventoryList.TryRemoveItem(FromPosition, FromEntry.ItemCount);
	InventoryList.TryAddItem(ToPosition, FromEntry.ItemInstance, FromEntry.ItemCount);
}

const TArray<FD1InventoryEntry>& UD1InventoryManagerComponent::GetAllItems() const
{
	return InventoryList.GetAllItems();
}

FD1InventoryEntry UD1InventoryManagerComponent::GetItemByPosition(const FIntPoint& ItemPosition)
{
	return InventoryList.GetItemByPosition(ItemPosition);
}

bool UD1InventoryManagerComponent::CanAddItemByPosition(const FIntPoint& ItemPosition, const FIntPoint& ItemSlotCount) const
{
	if (ItemPosition.X < 0 || ItemPosition.Y < 0)
		return false;
	
	if (ItemPosition.X + ItemSlotCount.X > InventorySlotCount.X || ItemPosition.Y + ItemSlotCount.Y > InventorySlotCount.Y)
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

bool UD1InventoryManagerComponent::CanMoveItemByPosition(const FIntPoint& FromPosition, const FIntPoint& ToPosition, const FIntPoint& ItemSlotCount) const
{
	if (FromPosition.X < 0 || FromPosition.Y < 0 || ToPosition.X < 0 || ToPosition.Y < 0)
		return false;
	
	if (FromPosition.X + ItemSlotCount.X > InventorySlotCount.X || FromPosition.Y + ItemSlotCount.Y > InventorySlotCount.Y ||
		ToPosition.X + ItemSlotCount.X > InventorySlotCount.X || ToPosition.Y + ItemSlotCount.Y > InventorySlotCount.Y)
		return false;
	
	TArray<TArray<bool>> TempSlotChecks = InventorySlotChecks;
	for (int32 y = FromPosition.Y; y < FromPosition.Y + ItemSlotCount.Y; y++)
	{
		for (int32 x = FromPosition.X; x < FromPosition.X + ItemSlotCount.X; x++)
		{
			TempSlotChecks[y][x] = false;
		}
	}

	for (int32 y = ToPosition.Y; y < ToPosition.Y + ItemSlotCount.Y; y++)
	{
		for (int32 x = ToPosition.X; x < ToPosition.X + ItemSlotCount.X; x++)
		{
			if (TempSlotChecks[y][x])
				return false;
		}
	}
	return true;
}

void UD1InventoryManagerComponent::MarkSlotChecks(bool bIsUsing, const FIntPoint& ItemPosition, const FIntPoint& ItemSlotCount)
{
	if (ItemPosition.X < 0 || ItemPosition.Y < 0)
		return;
	
	if (ItemPosition.X + ItemSlotCount.X > InventorySlotCount.X || ItemPosition.Y + ItemSlotCount.Y > InventorySlotCount.Y)
		return;
	
	for (int32 y = ItemPosition.Y; y < ItemPosition.Y + ItemSlotCount.Y; y++)
	{
		for (int32 x = ItemPosition.X; x < ItemPosition.X + ItemSlotCount.X; x++)
		{
			InventorySlotChecks[y][x] = bIsUsing;
		}
	}
}
