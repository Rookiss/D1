#include "D1InventoryManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1InventoryManagerComponent)

FString FD1InventoryEntry::GetDebugString() const
{
	return {};
}

void FD1InventoryList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	
}

void FD1InventoryList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	
}

void FD1InventoryList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	
}

UD1InventoryItemInstance* FD1InventoryList::AddEntry(TSubclassOf<UD1InventoryItemDefinition> ItemClass, int32 StackCount)
{
	return nullptr;
}

void FD1InventoryList::AddEntry(UD1InventoryItemInstance* Instance)
{
}

void FD1InventoryList::RemoveEntry(UD1InventoryItemInstance* Instance)
{
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

bool UD1InventoryManagerComponent::CanAddItem(TSubclassOf<UD1InventoryItemDefinition> ItemDef, int32 StackCount)
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

void UD1InventoryManagerComponent::RemoveItem(UD1InventoryItemInstance* ItemInstance)
{
}

TArray<UD1InventoryItemInstance*> UD1InventoryManagerComponent::GetAllItems() const
{
	return {};
}

UD1InventoryItemInstance* UD1InventoryManagerComponent::FindFirstItemStackByDefinition(
	TSubclassOf<UD1InventoryItemDefinition> ItemDef) const
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
