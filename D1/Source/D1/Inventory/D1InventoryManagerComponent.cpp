#include "D1InventoryManagerComponent.h"

#include "D1ItemInstance.h"
#include "Data/D1ItemData.h"
#include "Engine/ActorChannel.h"
#include "Fragments/D1ItemFragment.h"
#include "Fragments/D1ItemFragment_Stackable.h"
#include "Net/UnrealNetwork.h"
#include "System/D1AssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1InventoryManagerComponent)

// @TODO: OK
FString FD1InventoryEntry::GetDebugString() const
{
	return FString::Printf(TEXT("[%s]"), *ItemInstance->GetDebugString());
}

// @TODO: OK
bool FD1InventoryList::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
{
	return FFastArraySerializer::FastArrayDeltaSerialize<FD1InventoryEntry, FD1InventoryList>(Entries, DeltaParams, *this);
}

// @TODO: OK
void FD1InventoryList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	const FIntPoint& InventorySlotCount = OwnerComponent->GetInventorySlotCount();
	
	for (int32 Index : AddedIndices)
	{
		const FD1InventoryEntry& Entry = Entries[Index];
		if (Entry.GetItemInstance() == nullptr)
			continue;

		const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(Entry.GetItemInstance()->GetItemID());
		const FIntPoint ItemSlotPos = FIntPoint(Index % InventorySlotCount.X, Index / InventorySlotCount.X);
		const FIntPoint& ItemSlotCount = ItemDef.ItemSlotCount;

		OwnerComponent->MarkSlotChecks(true, ItemSlotPos, ItemSlotCount);
		
		if (OwnerComponent->OnInventoryEntryChanged.IsBound())
		{
			OwnerComponent->OnInventoryEntryChanged.Broadcast(ItemSlotPos, Entry.ItemInstance, Entry.ItemCount);
		}
	}
}

// @TODO: OK
void FD1InventoryList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
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
		const FIntPoint ItemSlotPos = FIntPoint(Index % InventorySlotCount.X, Index / InventorySlotCount.X);
		const FIntPoint& ItemSlotCount = ItemDef.ItemSlotCount;

		OwnerComponent->MarkSlotChecks(false, ItemSlotPos, ItemSlotCount);

		if (OwnerComponent->OnInventoryEntryChanged.IsBound())
		{
			OwnerComponent->OnInventoryEntryChanged.Broadcast(ItemSlotPos, Entry.ItemInstance, Entry.ItemCount);
		}
	}

	for (int32 Index : AliveIndices)
	{
		const FD1InventoryEntry& Entry = Entries[Index];
		
		const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID((Entry.GetItemInstance()->GetItemID()));
		const FIntPoint ItemSlotPos = FIntPoint(Index % InventorySlotCount.X, Index / InventorySlotCount.X);
		const FIntPoint& ItemSlotCount = ItemDef.ItemSlotCount;

		OwnerComponent->MarkSlotChecks(true, ItemSlotPos, ItemSlotCount);
		
		if (OwnerComponent->OnInventoryEntryChanged.IsBound())
		{
			OwnerComponent->OnInventoryEntryChanged.Broadcast(ItemSlotPos, Entry.ItemInstance, Entry.ItemCount);
		}
	}
}

// @TODO: 
bool FD1InventoryList::TryAddItem(const FIntPoint& ToItemSlotPos, UD1ItemInstance* FromItemInstance, int32 FromItemCount)
{
	AActor* OwningActor = OwnerComponent->GetOwner();
	if (OwningActor->HasAuthority() == false)
		return false;
	
	if (FromItemInstance == nullptr || FromItemCount <= 0)
		return false;

	const FIntPoint& InventorySlotCount = OwnerComponent->GetInventorySlotCount();
	if (ToItemSlotPos.X < 0 || ToItemSlotPos.Y < 0 || ToItemSlotPos.X >= InventorySlotCount.X || ToItemSlotPos.Y >= InventorySlotCount.Y)
		return false;
	
	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	const FD1ItemDefinition& FromItemDef = ItemData->GetItemDefByID(FromItemInstance->GetItemID());
	
	int32 Index = ToItemSlotPos.Y * InventorySlotCount.X + ToItemSlotPos.X;
	FD1InventoryEntry& ToEntry = Entries[Index];
	
	if (ToEntry.ItemInstance)
	{
		if (int32 CanStackCount = OwnerComponent->CanStackItem(FromItemInstance->ItemID, FromItemCount, ToItemSlotPos))
		{
			ToEntry.ItemCount += CanStackCount;
			MarkItemDirty(ToEntry);
			return true;
		}
	}
	else 
	{
		if (OwnerComponent->IsEmpty(ToItemSlotPos, FromItemDef.ItemSlotCount))
		{
			OwnerComponent->MarkSlotChecks(true, ToItemSlotPos, FromItemDef.ItemSlotCount);
	
			ToEntry.ItemInstance = FromItemInstance;
			ToEntry.ItemCount = FromItemCount;
			ToEntry.CachedItemID = FromItemInstance->GetItemID();
				
			MarkItemDirty(ToEntry);
			return true;
		}
	}
	return false;
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
			if (OwnerComponent->IsEmpty(ItemPosition, ItemSlotCount))
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
			if (OwnerComponent->IsEmpty(ItemPosition, ItemSlotCount))
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

bool FD1InventoryList::TryRemoveItem(const FIntPoint& ItemSlotPos, int32 ItemCount)
{
	// @TODO: Check Max Stack Count
	if (ItemCount <= 0)
		return false;

	if (ItemSlotPos.X < 0 || ItemSlotPos.Y < 0)
		return false;

	const FIntPoint& InventorySlotCount = OwnerComponent->GetInventorySlotCount();
	if (ItemSlotPos.X >= InventorySlotCount.X || ItemSlotPos.Y >= InventorySlotCount.Y)
		return false;
	
	check(OwnerComponent);
	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());
	
	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	check(ItemData);
	
	int32 Index = ItemSlotPos.Y * InventorySlotCount.X + ItemSlotPos.X;
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
		OwnerComponent->MarkSlotChecks(false, ItemSlotPos, ItemDef.ItemSlotCount);
		InventoryEntry.ItemInstance = nullptr;
	}
	MarkItemDirty(InventoryEntry);
	return true;
}

// bool FD1InventoryList::TryRemoveItem(int32 ItemID, int32 ItemCount)
// {
// 	// @TODO: Check Max Stack Count
// 	if (ItemID <= 0 || ItemCount <= 0)
// 		return false;
//
// 	check(OwnerComponent);
// 	AActor* OwningActor = OwnerComponent->GetOwner();
// 	check(OwningActor->HasAuthority());
// 	
// 	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
// 	check(ItemData);
//
// 	const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(ItemID);
// 	if (ItemDef.FindFragmentByClass<UD1ItemFragment_Stackable>() == nullptr)
// 		return false;
//
// 	if (ItemCount > GetTotalCountByID(ItemID))
// 		return false;
// 	
// 	for (int32 i = 0; i < Entries.Num(); i++)
// 	{
// 		FD1InventoryEntry& Entry = Entries[i];
// 		if (Entry.ItemInstance == nullptr)
// 			continue;
// 		
// 		if (Entry.ItemInstance->GetItemID() == ItemID)
// 		{
// 			if (Entry.ItemCount > ItemCount)
// 			{
// 				Entry.ItemCount -= ItemCount;
// 				MarkItemDirty(Entry);
// 				break;
// 			}
// 			else
// 			{
// 				ItemCount -= Entry.ItemCount;
// 				
// 				const FIntPoint& InventorySlotCount = OwnerComponent->GetInventorySlotCount();
// 				FIntPoint ItemPosition = FIntPoint(i % InventorySlotCount.X, i / InventorySlotCount.X);
// 				OwnerComponent->MarkSlotChecks(false, ItemPosition, ItemDef.ItemSlotCount);
//
// 				Entry.ItemInstance = nullptr;
// 				Entry.ItemCount = 0;
// 				MarkItemDirty(Entry);
// 				
// 				if (ItemCount <= 0)
// 					break;
// 			}
// 		}
// 	}
// 	return true;
// }

FD1InventoryEntry FD1InventoryList::GetItemByPosition(const FIntPoint& ItemSlotPos)
{
	if (ItemSlotPos.X < 0 || ItemSlotPos.Y < 0)
		return FD1InventoryEntry();

	const FIntPoint& InventorySlotCount = OwnerComponent->GetInventorySlotCount();
	if (ItemSlotPos.X >= InventorySlotCount.X || ItemSlotPos.Y >= InventorySlotCount.Y)
		return FD1InventoryEntry();
	
	int32 Index = ItemSlotPos.Y * InventorySlotCount.X + ItemSlotPos.X;
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
	InventoryList.Entries.Reserve(InventorySlotCount.X * InventorySlotCount.Y);
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

void UD1InventoryManagerComponent::Server_Init()
{
	if (GetOwner()->HasAuthority() == false)
		return;
	
	InventoryList.Entries.SetNum(InventorySlotCount.X * InventorySlotCount.Y);
}

bool UD1InventoryManagerComponent::TryAddItemByPosition(const FIntPoint& ItemSlotPos, UD1ItemInstance* ItemInstance, int32 ItemCount)
{
	if (GetOwner()->HasAuthority() == false)
		return false;
	
	return InventoryList.TryAddItem(ItemSlotPos, ItemInstance, ItemCount);
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

bool UD1InventoryManagerComponent::TryRemoveItemByPosition(const FIntPoint& ItemSlotPos, int32 ItemCount)
{
	if (GetOwner()->HasAuthority() == false)
		return false;
	
	return InventoryList.TryRemoveItem(ItemSlotPos, ItemCount);
}

bool UD1InventoryManagerComponent::TryRemoveItemByID(int32 ItemID, int32 ItemCount)
{
	if (GetOwner()->HasAuthority() == false)
		return false;
	
	return InventoryList.TryRemoveItem(ItemID, ItemCount);
}

void UD1InventoryManagerComponent::RequestMoveOrMergeItem_Implementation(const FIntPoint& FromSlotPos, const FIntPoint& ToSlotPos)
{
	if (GetOwner()->HasAuthority() == false)
		return;
	
	if (FromSlotPos == ToSlotPos)
		return;

	if (FromSlotPos.X < 0 || FromSlotPos.Y < 0 || ToSlotPos.X < 0 || ToSlotPos.Y < 0)
		return;

	if (FromSlotPos.X >= InventorySlotCount.X || FromSlotPos.Y >= InventorySlotCount.Y ||
		ToSlotPos.X >= InventorySlotCount.X || ToSlotPos.Y >= InventorySlotCount.Y)
		return;
	
	const FD1InventoryEntry FromEntry = InventoryList.GetItemByPosition(FromSlotPos);
	const FD1InventoryEntry ToEntry = InventoryList.GetItemByPosition(ToSlotPos);

	UD1ItemInstance* FromInstance = FromEntry.GetItemInstance();
	UD1ItemInstance* ToInstance = ToEntry.GetItemInstance();
	
	if (FromInstance && ToInstance)
	{
		if (FromInstance->GetItemID() != ToInstance->GetItemID())
			return;
		
		if (FromInstance->FindFragmentByClass<UD1ItemFragment_Stackable>() == nullptr)
			return;
		
		InventoryList.TryRemoveItem(FromSlotPos, FromEntry.ItemCount);
		if (InventoryList.TryAddItem(ToSlotPos, ToEntry.ItemInstance, FromEntry.ItemCount) == false)
		{
			InventoryList.TryAddItem(FromSlotPos, ToEntry.ItemInstance, FromEntry.ItemCount);
		}
	}
	else if (FromEntry.ItemInstance && ToEntry.ItemInstance == nullptr)
	{
		if (CanMoveItem(FromEntry.ItemInstance->GetItemID(), FromSlotPos, ToSlotPos) == false)
			return;

		InventoryList.TryRemoveItem(FromSlotPos, FromEntry.ItemCount);
		InventoryList.TryAddItem(ToSlotPos, FromEntry.ItemInstance, FromEntry.ItemCount);
	}
}

const TArray<FD1InventoryEntry>& UD1InventoryManagerComponent::GetAllEntries() const
{
	return InventoryList.GetAllEntries();
}

FD1InventoryEntry UD1InventoryManagerComponent::GetItemByPosition(const FIntPoint& ItemSlotPos)
{
	return InventoryList.GetItemByPosition(ItemSlotPos);
}

// @TODO: OK
bool UD1InventoryManagerComponent::IsEmpty(const FIntPoint& ItemSlotPos, const FIntPoint& ItemSlotCount) const
{
	if (ItemSlotPos.X < 0 || ItemSlotPos.Y < 0)
		return false;
	
	if (ItemSlotPos.X + ItemSlotCount.X > InventorySlotCount.X || ItemSlotPos.Y + ItemSlotCount.Y > InventorySlotCount.Y)
		return false;
	
	const FIntPoint StartSlotPos = ItemSlotPos;
	const FIntPoint EndSlotPos = ItemSlotPos + ItemSlotCount;
	
	for (int32 y = StartSlotPos.Y; y < EndSlotPos.Y; y++)
	{
		for (int32 x = StartSlotPos.X; x < EndSlotPos.X; x++)
		{
			if (InventorySlotChecks[y][x])
				return false;
		}
	}
	return true;
}

// @TODO: OK
bool UD1InventoryManagerComponent::CanMoveItem(const FIntPoint& FromSlotPos, const FIntPoint& ToSlotPos) const
{
	if (FromSlotPos.X < 0 || FromSlotPos.Y < 0 || FromSlotPos.X >= InventorySlotCount.X || FromSlotPos.Y >= InventorySlotCount.Y)
		return false;

	if (ToSlotPos.X < 0 || ToSlotPos.Y < 0 || ToSlotPos.X >= InventorySlotCount.X || ToSlotPos.Y >= InventorySlotCount.Y)
		return false;

	const TArray<FD1InventoryEntry>& Entries = GetAllEntries();
	
	int32 FromIndex = FromSlotPos.Y * InventorySlotCount.X + FromSlotPos.X;
	const FD1InventoryEntry& FromEntry = Entries[FromIndex];
	if (FromEntry.GetItemInstance() == nullptr)
		return false;

	int32 ToIndex = ToSlotPos.Y * InventorySlotCount.X + ToSlotPos.X;
	const FD1InventoryEntry& ToEntry = Entries[ToIndex];
	if (ToEntry.GetItemInstance())
		return false;

	const UD1ItemInstance* FromItemInstance = FromEntry.GetItemInstance();
	const int32 FromItemID = FromItemInstance->GetItemID();
	
	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	const FD1ItemDefinition& FromItemDef = ItemData->GetItemDefByID(FromItemID);
	const FIntPoint& FromItemSlotCount = FromItemDef.ItemSlotCount;
	
	if (ToSlotPos.X + FromItemSlotCount.X > InventorySlotCount.X || ToSlotPos.Y + FromItemSlotCount.Y > InventorySlotCount.Y)
		return false;

	if (FromSlotPos == ToSlotPos)
		return true;

	FIntPoint StartSlotPos = FromSlotPos;
	FIntPoint EndSlotPos = FromSlotPos + FromItemSlotCount;
	
	TArray<TArray<bool>> TempSlotChecks = InventorySlotChecks;
	for (int32 y = StartSlotPos.Y; y < EndSlotPos.Y; y++)
	{
		for (int32 x = StartSlotPos.X; x < EndSlotPos.X; x++)
		{
			TempSlotChecks[y][x] = false;
		}
	}
	
	StartSlotPos = ToSlotPos;
	EndSlotPos = ToSlotPos + FromItemSlotCount;
	
	for (int32 y = StartSlotPos.Y; y < EndSlotPos.Y; y++)
	{
		for (int32 x = StartSlotPos.X; x < EndSlotPos.X; x++)
		{
			if (TempSlotChecks[y][x])
				return false;
		}
	}
	return true;
}

// @TODO: 
bool UD1InventoryManagerComponent::CanMergeItem(const FIntPoint& FromSlotPos, const FIntPoint& ToSlotPos, int32& MergeCount) const
{
	if (FromSlotPos == ToSlotPos)
		return false;

	if (FromSlotPos.X < 0 || FromSlotPos.Y < 0 || FromSlotPos.X >= InventorySlotCount.X || FromSlotPos.Y >= InventorySlotCount.Y)
		return false;

	if (ToSlotPos.X < 0 || ToSlotPos.Y < 0 || ToSlotPos.X >= InventorySlotCount.X || ToSlotPos.Y >= InventorySlotCount.Y)
		return false;

	const TArray<FD1InventoryEntry>& Entries = GetAllEntries();
	
	int32 FromIndex = FromSlotPos.Y * InventorySlotCount.X + FromSlotPos.X;
	const FD1InventoryEntry& FromEntry = Entries[FromIndex];
	if (FromEntry.GetItemInstance() == nullptr)
		return false;

	int32 ToIndex = ToSlotPos.Y * InventorySlotCount.X + ToSlotPos.X;
	const FD1InventoryEntry& ToEntry = Entries[ToIndex];
	if (ToEntry.GetItemInstance() == nullptr)
		return false;
	
	const UD1ItemInstance* FromItemInstance = FromEntry.GetItemInstance();
	const int32 FromItemID = FromItemInstance->GetItemID();

	const UD1ItemInstance* ToItemInstance = ToEntry.GetItemInstance();
	const int32 ToItemID = ToItemInstance->GetItemID();

	if (FromItemID != ToItemID)
		return false;
	
	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	const FD1ItemDefinition& FromItemDef = ItemData->GetItemDefByID(FromItemID);
	const UD1ItemFragment_Stackable* Stackable = FromItemDef.FindFragmentByClass<UD1ItemFragment_Stackable>();
	if (Stackable == nullptr)
		return false;

	// TODO
	// MergeCount = FMath::Max(0, FromEntry.GetItemCount() + ToEntry.GetItemCount() - Stackable->MaxStackCount);
	return MergeCount;
}

// @TODO: OK
void UD1InventoryManagerComponent::MarkSlotChecks(bool bIsUsing, const FIntPoint& ItemSlotPos, const FIntPoint& ItemSlotCount)
{
	if (ItemSlotPos.X < 0 || ItemSlotPos.Y < 0)
		return;
	
	if (ItemSlotPos.X + ItemSlotCount.X > InventorySlotCount.X || ItemSlotPos.Y + ItemSlotCount.Y > InventorySlotCount.Y)
		return;

	const FIntPoint StartSlotPos = ItemSlotPos;
	const FIntPoint EndSlotPos = ItemSlotPos + ItemSlotCount;
	
	for (int32 y = StartSlotPos.Y; y < EndSlotPos.Y; y++)
	{
		for (int32 x = StartSlotPos.X; x < EndSlotPos.X; x++)
		{
			InventorySlotChecks[y][x] = bIsUsing;
		}
	}
}
