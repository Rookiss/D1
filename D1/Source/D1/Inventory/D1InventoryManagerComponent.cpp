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
	return FString::Printf(TEXT("[%s] [Pos : (%d,%d)]"), *Instance->GetDebugString(), Position.X, Position.Y);
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
			OwnerComponent->OnInventoryEntryChanged.Broadcast(Entry.Position, Entry.Instance, 0);
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
			OwnerComponent->OnInventoryEntryChanged.Broadcast(Entry.Position, Entry.Instance, Entry.Count);
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
			OwnerComponent->OnInventoryEntryChanged.Broadcast(Entry.Position, Entry.Instance, Entry.Count);
		}
	}
}

bool FD1InventoryList::TryAddItem(int32 ItemID, int32 Count)
{
	// @TODO: Check Max Stack Count
	if (ItemID <= 0 || Count <= 0)
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
			if (Entry.Instance->GetItemID() == ItemID)
			{
				Entry.Count += Count;
				MarkItemDirty(Entry);
				return true;
			}
		}
	}
	
	const FIntPoint& SlotSize = ItemDef.SlotSize;
	TArray<TArray<bool>>& SlotChecks = OwnerComponent->GetSlotChecks();
	for (int32 Y = 0; Y < SlotChecks.Num(); Y++)
	{
		for (int32 X = 0; X < SlotChecks[Y].Num(); X++)
		{
			if (SlotChecks[Y][X])
				continue;

			FIntPoint Position = FIntPoint(X, Y);
			if (OwnerComponent->CanAddItemByPosition(Position, SlotSize))
			{
				OwnerComponent->MarkSlotChecks(true, Position, SlotSize);
				
				FD1InventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
				NewEntry.Instance = NewObject<UD1ItemInstance>(OwnerComponent->GetOwner());
				NewEntry.Instance->SetItemID(ItemID);
				NewEntry.Count = Count;
				NewEntry.Position = Position;

				for (const UD1ItemFragment* Fragment : ItemDef.Fragments)
				{
					if (Fragment)
					{
						Fragment->OnInstanceCreated(NewEntry.Instance);
					}
				}
				
				MarkItemDirty(NewEntry);
				OwnerComponent->AddReplicatedSubObject(NewEntry.Instance);
				return true;
			}
		}
	}
	return false;
}

bool FD1InventoryList::TryAddItem(UD1ItemInstance* Instance, int32 Count)
{
	// @TODO: Check Max Stack Count
	if (Instance == nullptr || Count <= 0)
		return false;
	
	check(OwnerComponent);
	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	check(ItemData);

	const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(Instance->GetItemID());
	if (ItemDef.FindFragmentByClass<UD1ItemFragment_Stackable>())
	{
		for (FD1InventoryEntry& Entry : Entries)
		{
			if (Entry.Instance->GetItemID() == Instance->GetItemID())
			{
				Entry.Count += Count;
				MarkItemDirty(Entry);
				return true;
			}
		}
	}
	
	const FIntPoint& SlotSize = ItemDef.SlotSize;
	TArray<TArray<bool>>& SlotChecks = OwnerComponent->GetSlotChecks();
	for (int32 Y = 0; Y < SlotChecks.Num(); Y++)
	{
		for (int32 X = 0; X < SlotChecks[Y].Num(); X++)
		{
			if (SlotChecks[Y][X])
				continue;

			FIntPoint Position = FIntPoint(X, Y);
			if (OwnerComponent->CanAddItemByPosition(Position, SlotSize))
			{
				OwnerComponent->MarkSlotChecks(true, Position, SlotSize);
				
				FD1InventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
				NewEntry.Instance = Instance;
				NewEntry.Count = Count;
				NewEntry.Position = Position;
				
				MarkItemDirty(NewEntry);
				OwnerComponent->AddReplicatedSubObject(NewEntry.Instance);
				return true;
			}
		}
	}
	return false;
}

bool FD1InventoryList::TryRemoveItem(int32 ItemID, int32 Count)
{
	// @TODO: Check Max Stack Count
	if (ItemID <= 0 || Count <= 0)
		return false;

	check(OwnerComponent);
	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());
	
	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	check(ItemData);

	const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(ItemID);
	if (ItemDef.FindFragmentByClass<UD1ItemFragment_Stackable>() == nullptr)
		return false;

	if (Count > GetTotalCountByID(ItemID))
		return false;
	
	for (auto It = Entries.CreateIterator(); It; ++It)
	{
		FD1InventoryEntry& Entry = *It;
		if (Entry.Instance->GetItemID() == ItemID)
		{
			if (Entry.Count > Count)
			{
				Entry.Count -= Count;
				MarkItemDirty(Entry);
				break;
			}
			else
			{
				Count -= Entry.Count;
				OwnerComponent->MarkSlotChecks(false, Entry.Position, ItemDef.SlotSize);
				OwnerComponent->RemoveReplicatedSubObject(Entry.Instance);
				It.RemoveCurrent();
				MarkArrayDirty();
				
				if (Count <= 0)
					break;
			}
		}
	}
	return true;
}

bool FD1InventoryList::TryRemoveItem(UD1ItemInstance* Instance, int32 Count)
{
	// @TODO: Check Max Stack Count
	if (Instance == nullptr)
		return false;
	
	check(OwnerComponent);
	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	check(ItemData);

	const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(Instance->GetItemID());
	
	if (Count > GetTotalCountByInstance(Instance))
		return false;
	
	for (auto It = Entries.CreateIterator(); It; ++It)
	{
		FD1InventoryEntry& Entry = *It;
		if (Entry.Instance == Instance)
		{
			if (Entry.Count > Count)
			{
				Entry.Count -= Count;
				MarkItemDirty(Entry);
				break;
			}
			else
			{
				Count -= Entry.Count;
				OwnerComponent->MarkSlotChecks(false, Entry.Position, ItemDef.SlotSize);
				OwnerComponent->RemoveReplicatedSubObject(Entry.Instance);
				It.RemoveCurrent();
				MarkArrayDirty();
				
				if (Count <= 0)
					break;
			}
		}
	}
	return true;
}

int32 FD1InventoryList::GetTotalCountByID(int32 ItemID)
{
	int32 Result = 0;
	for (FD1InventoryEntry& Entry : Entries)
	{
		if (Entry.Instance->GetItemID() == ItemID)
		{
			Result += Entry.Count;
		}
	}
	return Result;
}

int32 FD1InventoryList::GetTotalCountByInstance(UD1ItemInstance* Instance)
{
	int32 Result = 0;
	for (FD1InventoryEntry& Entry : Entries)
	{
		if (Entry.Instance == Instance)
		{
			Result += Entry.Count;
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

void UD1InventoryManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, InventoryList);
	DOREPLIFETIME(ThisClass, InventorySize);
}

void UD1InventoryManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	if (IsUsingRegisteredSubObjectList())
	{
		for (const FD1InventoryEntry& Entry : InventoryList.Entries)
		{
			UD1ItemInstance* Instance = Entry.Instance;
			if (IsValid(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}
	}
}

bool UD1InventoryManagerComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	for (FD1InventoryEntry& Entry : InventoryList.Entries)
	{
		UD1ItemInstance* Instance = Entry.Instance;
		if (Instance && IsValid(Instance))
		{
			WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}
	return WroteSomething;
}

bool UD1InventoryManagerComponent::TryExpandInventorySize(const FIntPoint& NewSize)
{
	if (InventorySize.X > NewSize.X || InventorySize.Y > NewSize.Y)
		return false;
	
	InventorySize = NewSize;
	
	SlotChecks.SetNumZeroed(InventorySize.Y);
	for (int32 Y = 0; Y < SlotChecks.Num(); Y++)
	{
		SlotChecks[Y].SetNumZeroed(InventorySize.X);
	}
	return true;
}

bool UD1InventoryManagerComponent::TryAddItemByID(int32 ItemID, int32 Count)
{
	return InventoryList.TryAddItem(ItemID, Count);
}

bool UD1InventoryManagerComponent::TryAddItemByInstance(UD1ItemInstance* Instance, int32 Count)
{
	return InventoryList.TryAddItem(Instance, Count);
}

bool UD1InventoryManagerComponent::TryRemoveItemByID(int32 ItemID, int32 Count)
{
	return InventoryList.TryRemoveItem(ItemID, Count);
}

bool UD1InventoryManagerComponent::TryRemoveItemByInstance(UD1ItemInstance* Instance, int32 Count)
{
	return InventoryList.TryRemoveItem(Instance, Count);
}

const TArray<FD1InventoryEntry>& UD1InventoryManagerComponent::GetAllItems() const
{
	return InventoryList.GetAllItems();
}

bool UD1InventoryManagerComponent::CanAddItemByPosition(const FIntPoint& Position, const FIntPoint& SlotSize)
{
	for (int32 y = Position.Y; y < Position.Y + SlotSize.Y; y++)
	{
		for (int32 x = Position.X; x < Position.X + SlotSize.X; x++)
		{
			if (SlotChecks[y][x])
				return false;
		}
	}
	return true;
}

void UD1InventoryManagerComponent::MarkSlotChecks(bool bIsUsing, const FIntPoint& Position, const FIntPoint& SlotSize)
{
	for (int32 y = Position.Y; y < Position.Y + SlotSize.Y; y++)
	{
		for (int32 x = Position.X; x < Position.X + SlotSize.X; x++)
		{
			SlotChecks[y][x] = bIsUsing;
		}
	}
}

void UD1InventoryManagerComponent::AddReplicatedSubObject(UD1ItemInstance* Instance)
{
	if (Instance && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		Super::AddReplicatedSubObject(Instance);
	}
}

void UD1InventoryManagerComponent::RemoveReplicatedSubObject(UD1ItemInstance* Instance)
{
	if (Instance && IsUsingRegisteredSubObjectList())
	{
		Super::RemoveReplicatedSubObject(Instance);
	}
}
