#include "D1InventoryManagerComponent.h"

#include "D1InventoryItemDefinition.h"
#include "D1InventoryItemInstance.h"
#include "Fragments/InventoryItemFragment.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1InventoryManagerComponent)

FString FD1InventoryEntry::GetDebugString() const
{
	TSubclassOf<UD1InventoryItemDefinition> ItemDef;
	if (Instance)
	{
		ItemDef = Instance->GetItemDef();
	}
	return FString::Printf(TEXT("%s | %d | %s"), *GetNameSafe(Instance), StackCount, *GetNameSafe(ItemDef));
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

UD1InventoryItemInstance* FD1InventoryList::AddEntry(TSubclassOf<UD1InventoryItemDefinition> ItemDef, int32 StackCount)
{
	check(ItemDef);
	check(OwnerComponent);

	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	FD1InventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Instance = NewObject<UD1InventoryItemInstance>(OwnerComponent->GetOwner());
	NewEntry.Instance->SetItemDef(ItemDef);

	for (UInventoryItemFragment* Fragment : GetDefault<UD1InventoryItemDefinition>(ItemDef)->Fragments)
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

void FD1InventoryList::RemoveEntry(UD1InventoryItemInstance* Instance)
{
	for (auto It = Entries.CreateIterator(); It; ++It)
	{
		FD1InventoryEntry& Entry = *It;
		if (Entry.Instance == Instance)
		{
			It.RemoveCurrent();
			MarkArrayDirty();
		}
	}
}

TArray<UD1InventoryItemInstance*> FD1InventoryList::GetAllItems() const
{
	return {};
}

UD1InventoryManagerComponent::UD1InventoryManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1InventoryManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

bool UD1InventoryManagerComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch,
	FReplicationFlags* RepFlags)
{
	return Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
}

void UD1InventoryManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();
}

bool UD1InventoryManagerComponent::CanAddItemByDefinition(TSubclassOf<UD1InventoryItemDefinition> ItemDef, int32 StackCount)
{
	return false;
}

UD1InventoryItemInstance* UD1InventoryManagerComponent::AddItemByDefinition(
	TSubclassOf<UD1InventoryItemDefinition> ItemDef, int32 StackCount)
{
	return nullptr;
}

void UD1InventoryManagerComponent::AddItemByInstance(UD1InventoryItemInstance* ItemInstance)
{
}

void UD1InventoryManagerComponent::RemoveItemByInstance(UD1InventoryItemInstance* ItemInstance)
{
}

TArray<UD1InventoryItemInstance*> UD1InventoryManagerComponent::GetAllItems() const
{
	return {};
}

UD1InventoryItemInstance* UD1InventoryManagerComponent::FindFirstItemByDefinition(TSubclassOf<UD1InventoryItemDefinition> ItemDef) const
{
	return nullptr;
}

int32 UD1InventoryManagerComponent::GetTotalItemCountByDefinition(TSubclassOf<UD1InventoryItemDefinition> ItemDef) const
{
	return 0;
}

bool UD1InventoryManagerComponent::ConsumeItemsByDefinition(TSubclassOf<UD1InventoryItemDefinition> ItemDef,
	int32 Count)
{
	return false;
}
