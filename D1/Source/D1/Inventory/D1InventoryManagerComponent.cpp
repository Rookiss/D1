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
	const UD1ItemData* ItemData = UD1AssetManager::GetAssetByName<UD1ItemData>("ItemData");
	check(ItemData);
	
	const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(Instance->GetItemID());
	return FString::Printf(TEXT("%d | %s x %d"), Instance->GetItemID(), *ItemDef.DisplayName.ToString(), StackCount);
}

bool FD1InventoryList::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
{
	return FFastArraySerializer::FastArrayDeltaSerialize<FD1InventoryEntry, FD1InventoryList>(Entries, DeltaParams, *this);
}

void FD1InventoryList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (int32 Index : RemovedIndices)
	{
		FD1InventoryEntry& Entry = Entries[Index];
		// TODO: Broadcast(Entry.StackCount -> 0)
		Entry.LastObservedCount = 0;
	}
}

void FD1InventoryList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		FD1InventoryEntry& Entry = Entries[Index];
		// TODO: Broadcast(0 -> Entry.StackCount)
		Entry.LastObservedCount = Entry.StackCount;
	}
}

void FD1InventoryList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		FD1InventoryEntry& Entry = Entries[Index];
		check(Entry.LastObservedCount != INDEX_NONE);
		// TODO: Broadcast(Entry.LastObservedCount -> Entry.StackCount)
		Entry.LastObservedCount = Entry.StackCount;
	}
}

UD1ItemInstance* FD1InventoryList::TryAddItem(int32 ItemID, int32 StackCount)
{
	check(OwnerComponent);
	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	const UD1ItemData* ItemData = UD1AssetManager::GetAssetByName<UD1ItemData>("ItemData");
	check(ItemData);

	const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(ItemID);
	if (ItemDef.FindFragmentByClass(UD1ItemFragment_Stackable::StaticClass()))
	{
		for (FD1InventoryEntry& Entry : Entries)
		{
			if (Entry.Instance->GetItemID() == ItemID)
			{
				Entry.StackCount += StackCount;
				MarkItemDirty(Entry);
				return Entry.Instance;
			}
		}
	}

	if (Entries.Num() >= InventorySize)
	{
		return nullptr;
	}
	
	FD1InventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Instance = NewObject<UD1ItemInstance>(OwnerComponent->GetOwner());
	NewEntry.Instance->SetItemID(ItemID);

	for (const UD1ItemFragment* Fragment : ItemDef.Fragments)
	{
		if (Fragment)
		{
			Fragment->OnInstanceCreated(NewEntry.Instance);
		}
	}
	
	NewEntry.StackCount = StackCount;
	MarkItemDirty(NewEntry);
	return NewEntry.Instance;
}

UD1ItemInstance* FD1InventoryList::TryAddItem(UD1ItemInstance* Instance, int32 StackCount)
{
	check(OwnerComponent);
	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	const UD1ItemData* ItemData = UD1AssetManager::GetAssetByName<UD1ItemData>("ItemData");
	check(ItemData);
	
	const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(Instance->GetItemID());
	if (ItemDef.FindFragmentByClass(UD1ItemFragment_Stackable::StaticClass()))
	{
		for (FD1InventoryEntry& Entry : Entries)
		{
			if (Entry.Instance->GetItemID() == Instance->GetItemID())
			{
				Entry.StackCount += StackCount;
				MarkItemDirty(Entry);
				return Entry.Instance;
			}
		}
	}

	if (Entries.Num() >= InventorySize)
	{
		return nullptr;
	}

	FD1InventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Instance = Instance;
	NewEntry.StackCount = StackCount;
	MarkItemDirty(NewEntry);
	return NewEntry.Instance;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

bool FD1InventoryList::TryRemoveItem(UD1ItemInstance* Instance, int32 StackCount)
{
	for (auto It = Entries.CreateIterator(); It; ++It)
	{
		FD1InventoryEntry& Entry = *It;
		if (Entry.Instance == Instance)
		{
			if (Entry.StackCount > StackCount)
			{
				Entry.StackCount -= StackCount;
				MarkItemDirty(Entry);
			}
			else if (Entry.StackCount == StackCount)
			{
				It.RemoveCurrent();
				MarkArrayDirty();
			}
			else
			{
				UE_LOG(LogD1, Fatal, TEXT("Lack of StackCount : [%d] - [%d]"), Entry.StackCount, StackCount);
			}
			break;
		}
	}
}

UD1InventoryManagerComponent::UD1InventoryManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    SetIsReplicatedByDefault(true);
}

void UD1InventoryManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, InventoryList);
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

void UD1InventoryManagerComponent::SetInventorySize(int32 NewSize)
{
	if (NewSize <= InventoryList.GetInventorySize())
		return;

	InventoryList.SetInventorySize(NewSize);
}

bool UD1InventoryManagerComponent::CanAddItem(int32 ItemID, int32 StackCount)
{
	
	return false;
}
