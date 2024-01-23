#include "D1InventoryManagerComponent.h"

#include "Data/D1ItemData.h"
#include "Engine/ActorChannel.h"
#include "Item/D1ItemInstance.h"
#include "Item/Fragments/D1ItemFragment_Stackable.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "System/D1AssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1InventoryManagerComponent)

void FD1InventoryEntry::Init(int32 InItemID, int32 InItemCount)
{
	if (InItemID <= 0)
		return;
	
	ItemInstance = NewObject<UD1ItemInstance>();
	ItemInstance->Init(InItemID);

	const UD1ItemFragment_Stackable* Stackable = ItemInstance->FindFragmentByClass<UD1ItemFragment_Stackable>();
	InItemCount = Stackable ? FMath::Clamp(InItemCount, 1, Stackable->MaxStackCount) : 1;
	
	ItemCount = InItemCount;
	LatestValidItemID = InItemID;
}

void FD1InventoryEntry::Init(UD1ItemInstance* InItemInstance, int32 InItemCount)
{
	if (InItemInstance == nullptr || InItemCount <= 0)
		return;
	
	ItemInstance = InItemInstance;
	
	const UD1ItemFragment_Stackable* Stackable = ItemInstance->FindFragmentByClass<UD1ItemFragment_Stackable>();
	ItemCount = Stackable ? FMath::Clamp(InItemCount, 1, Stackable->MaxStackCount) : 1;
		
	LatestValidItemID = ItemInstance->ItemID;
}

void FD1InventoryEntry::Reset()
{
	ItemInstance = nullptr;
	ItemCount = 0;
}

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
	const FIntPoint& InventorySlotCount = OwnerComponent->GetInventorySlotCount();

	for (int32 Index : AddedIndices)
	{
		const FD1InventoryEntry& Entry = Entries[Index];
		if (Entry.ItemInstance == nullptr)
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

		const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(Entry.LatestValidItemID);
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

void FD1InventoryList::InitOnServer()
{
	const FIntPoint& InventorySlotCount = OwnerComponent->GetInventorySlotCount();
	Entries.SetNum(InventorySlotCount.X * InventorySlotCount.Y);
	
	for (FD1InventoryEntry& Entry : Entries)
	{
		MarkItemDirty(Entry);
	}
}

bool FD1InventoryList::TryAddItem(const FIntPoint& ItemSlotPos, int32 ItemID, int32 ItemCount)
{
	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	if (ItemID <= 0 || ItemCount <= 0)
		return false;

	const FIntPoint& InventorySlotCount = OwnerComponent->GetInventorySlotCount();
	if (ItemSlotPos.X < 0 || ItemSlotPos.Y < 0 || ItemSlotPos.X >= InventorySlotCount.X || ItemSlotPos.Y >= InventorySlotCount.Y)
		return false;

	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(ItemID);

	int32 Index = ItemSlotPos.Y * InventorySlotCount.X + ItemSlotPos.X;
	FD1InventoryEntry& Entry = Entries[Index];
	
	if (Entry.ItemInstance)
	{
		if (Entry.ItemInstance->ItemID == ItemID)
		{
			if (const UD1ItemFragment_Stackable* Stackable = ItemDef.FindFragmentByClass<UD1ItemFragment_Stackable>())
			{
				if (Entry.ItemCount + ItemCount <= Stackable->MaxStackCount)
				{
					Entry.ItemCount += ItemCount;
					MarkItemDirty(Entry);
					return true;
				}
			}
		}
	}
	else
	{
		if (OwnerComponent->IsEmpty(ItemSlotPos, ItemDef.ItemSlotCount))
		{
			OwnerComponent->MarkSlotChecks(true, ItemSlotPos, ItemDef.ItemSlotCount);
			Entry.Init(ItemID, ItemCount);
			MarkItemDirty(Entry);
			return true;
		}
	}
	return false;
}

bool FD1InventoryList::TryAddItem(int32 ItemID, int32 ItemCount)
{
	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	if (ItemID <= 0 || ItemCount <= 0)
		return false;
	
	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(ItemID);
	const UD1ItemFragment_Stackable* Stackable = ItemDef.FindFragmentByClass<UD1ItemFragment_Stackable>();
	
	TArray<TPair<int32/*EntryIndex*/, int32/*AddCount*/>> JobQueue;
	
	if (Stackable)
	{
		for (int32 i = 0; i < Entries.Num(); i++)
		{
			const FD1InventoryEntry& Entry = Entries[i];
			
			if (Entry.ItemInstance == nullptr)
				continue;

			if (Entry.ItemInstance->GetItemID() != ItemID)
				continue;
			
			if (int32 AddCount = FMath::Min(Entry.GetItemCount() + ItemCount, Stackable->MaxStackCount) - Entry.GetItemCount())
			{
				JobQueue.Emplace(i, AddCount);
				ItemCount -= AddCount;

				if (ItemCount == 0)
				{
					for (const auto& Pair : JobQueue)
					{
						FD1InventoryEntry& ToEntry = Entries[Pair.Key];
						ToEntry.ItemCount += Pair.Value;
						MarkItemDirty(ToEntry);
					}
					return true;
				}
			}
		}
	}
	
	const FIntPoint& ItemSlotCount = ItemDef.ItemSlotCount;
	const FIntPoint& InventorySlotCount = OwnerComponent->GetInventorySlotCount();
	TArray<TArray<bool>> TempSlotChecks = OwnerComponent->GetSlotChecks();
	
	FIntPoint StartSlotPos = FIntPoint::ZeroValue;
	FIntPoint EndSlotPos = InventorySlotCount - ItemSlotCount;
	
	for (int32 y = StartSlotPos.Y; y <= EndSlotPos.Y; y++)
	{
		for (int32 x = StartSlotPos.X; x <= EndSlotPos.X; x++)
		{
			if (TempSlotChecks[y][x])
				continue;

			FIntPoint ItemSlotPos = FIntPoint(x, y);
			if (OwnerComponent->IsEmpty(TempSlotChecks, ItemSlotPos, ItemSlotCount))
			{
				OwnerComponent->MarkSlotChecks(TempSlotChecks, true, ItemSlotPos, ItemSlotCount);
				
				int32 AddCount = Stackable ? FMath::Min(ItemCount, Stackable->MaxStackCount) : 1;
				JobQueue.Emplace(y * InventorySlotCount.X + x, AddCount);
				ItemCount -= AddCount;
				
				if (ItemCount == 0)
				{
					for (const auto& Pair : JobQueue)
					{
						FIntPoint ToSlotPos = FIntPoint(Pair.Key % InventorySlotCount.X, Pair.Key / InventorySlotCount.X);
						OwnerComponent->MarkSlotChecks(true, ToSlotPos, ItemSlotCount);
						
						FD1InventoryEntry& Entry = Entries[Pair.Key];
						Entry.Init(ItemID, Pair.Value);

						MarkItemDirty(Entry);
					}
					return true;
				}
			}
		}
	}
	return false;
}

bool FD1InventoryList::TryRemoveItem(const FIntPoint& ItemSlotPos, int32 ItemCount)
{
	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());
	
	if (ItemCount <= 0)
		return false;
	
	const FIntPoint& InventorySlotCount = OwnerComponent->GetInventorySlotCount();
	if (ItemSlotPos.X < 0 || ItemSlotPos.Y < 0 || ItemSlotPos.X >= InventorySlotCount.X || ItemSlotPos.Y >= InventorySlotCount.Y)
		return false;

	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();

	int32 Index = ItemSlotPos.Y * InventorySlotCount.X + ItemSlotPos.X;
	FD1InventoryEntry& Entry = Entries[Index];
	if (Entry.ItemInstance == nullptr)
		return false;

	if (Entry.ItemCount < ItemCount)
		return false;

	Entry.ItemCount -= ItemCount;
	if (Entry.ItemCount == 0)
	{
		const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(Entry.ItemInstance->GetItemID());
		OwnerComponent->MarkSlotChecks(false, ItemSlotPos, ItemDef.ItemSlotCount);
		Entry.Reset();
	}
	MarkItemDirty(Entry);
	return true;
}

bool FD1InventoryList::TryRemoveItem(int32 ItemID, int32 ItemCount)
{
	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());
	
	if (ItemID <= 0 || ItemCount <= 0)
		return false;

	if (GetTotalCountByID(ItemID) < ItemCount)
		return false;
	
	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(ItemID);

	TArray<TPair<int32/*EntryIndex*/, int32/*RemoveCount*/>> JobQueue;
	
	for (int32 i = Entries.Num() - 1; i >= 0; i--)
	{
		const FD1InventoryEntry& Entry = Entries[i];
		if (Entry.ItemInstance == nullptr)
			continue;

		if (Entry.ItemInstance->GetItemID() != ItemID)
			continue;
			
		if (Entry.ItemCount < ItemCount)
		{
			JobQueue.Emplace(i, Entry.ItemCount);
			ItemCount -= Entry.ItemCount;
		}
		else
		{
			JobQueue.Emplace(i, ItemCount);
			
			const FIntPoint& InventorySlotCount = OwnerComponent->GetInventorySlotCount();
			for(const auto& Pair : JobQueue)
			{
				FD1InventoryEntry& ToEntry = Entries[Pair.Key];
				ToEntry.ItemCount -= Pair.Value;

				if (ToEntry.ItemCount == 0)
				{
					FIntPoint ItemPosition = FIntPoint(Pair.Key % InventorySlotCount.X, Pair.Key / InventorySlotCount.X);
					OwnerComponent->MarkSlotChecks(false, ItemPosition, ItemDef.ItemSlotCount);
					ToEntry.Reset();
				}
				MarkItemDirty(ToEntry);
			}
			return true;
		}
	}
	return false;
}

void FD1InventoryList::Unsafe_MoveItem(const FIntPoint& FromSlotPos, const FIntPoint& ToSlotPos)
{
	if (FromSlotPos == ToSlotPos)
		return;

	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	const FIntPoint& InventorySlotCount = OwnerComponent->GetInventorySlotCount();

	int32 FromIndex = FromSlotPos.Y * InventorySlotCount.X + FromSlotPos.X;
	FD1InventoryEntry& FromEntry = Entries[FromIndex];
	UD1ItemInstance* FromItemInstance = FromEntry.GetItemInstance();
	const int32 FromItemID = FromItemInstance->GetItemID();
	const FD1ItemDefinition& FromItemDef = ItemData->GetItemDefByID(FromItemID);

	int32 ToIndex = ToSlotPos.Y * InventorySlotCount.X + ToSlotPos.X;
	FD1InventoryEntry& ToEntry = Entries[ToIndex];

	ToEntry.ItemInstance = FromItemInstance;
	ToEntry.ItemCount = FromEntry.ItemCount;
	ToEntry.LatestValidItemID = FromItemInstance->ItemID;
	MarkItemDirty(ToEntry);

	FromEntry.Reset();
	MarkItemDirty(FromEntry);

	OwnerComponent->MarkSlotChecks(false, FromSlotPos, FromItemDef.ItemSlotCount);
	OwnerComponent->MarkSlotChecks(true, ToSlotPos, FromItemDef.ItemSlotCount);
}

void FD1InventoryList::Unsafe_MergeItem(const FIntPoint& FromSlotPos, const FIntPoint& ToSlotPos, int32 MergeCount)
{
	if (FromSlotPos == ToSlotPos || MergeCount == 0)
		return;

	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	const FIntPoint& InventorySlotCount = OwnerComponent->GetInventorySlotCount();

	int32 FromIndex = FromSlotPos.Y * InventorySlotCount.X + FromSlotPos.X;
	FD1InventoryEntry& FromEntry = Entries[FromIndex];
	UD1ItemInstance* FromItemInstance = FromEntry.GetItemInstance();
	const int32 FromItemID = FromItemInstance->GetItemID();
	const FD1ItemDefinition& FromItemDef = ItemData->GetItemDefByID(FromItemID);

	int32 ToIndex = ToSlotPos.Y * InventorySlotCount.X + ToSlotPos.X;
	FD1InventoryEntry& ToEntry = Entries[ToIndex];

	ToEntry.ItemCount += MergeCount;
	MarkItemDirty(ToEntry);

	FromEntry.ItemCount -= MergeCount;
	if (FromEntry.ItemCount <= 0)
	{
		FromEntry.Reset();
		OwnerComponent->MarkSlotChecks(false, FromSlotPos, FromItemDef.ItemSlotCount);
	}
	MarkItemDirty(FromEntry);
}

FD1InventoryEntry FD1InventoryList::GetEntryByPosition(const FIntPoint& ItemSlotPos) const
{
	if (ItemSlotPos.X < 0 || ItemSlotPos.Y < 0)
		return FD1InventoryEntry();

	const FIntPoint& InventorySlotCount = OwnerComponent->GetInventorySlotCount();
	if (ItemSlotPos.X >= InventorySlotCount.X || ItemSlotPos.Y >= InventorySlotCount.Y)
		return FD1InventoryEntry();

	return Entries[ItemSlotPos.Y * InventorySlotCount.X + ItemSlotPos.X];
}

int32 FD1InventoryList::GetTotalCountByID(int32 ItemID) const
{
	int32 Count = 0;
	for (const FD1InventoryEntry& Entry : Entries)
	{
		if (Entry.ItemInstance && Entry.ItemInstance->GetItemID() == ItemID)
		{
			Count += Entry.ItemCount;
		}
	}
	return Count;
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

void UD1InventoryManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		InventoryList.InitOnServer();
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
		UD1ItemInstance* ItemInstance = Entry.ItemInstance;
		if (IsValid(ItemInstance))
		{
			bWroteSomething |= Channel->ReplicateSubobject(ItemInstance, *Bunch, *RepFlags);
		}
	}
	return bWroteSomething;
}

void UD1InventoryManagerComponent::Server_RequestMoveOrMergeItem_Implementation(const FIntPoint& FromSlotPos, const FIntPoint& ToSlotPos)
{
	check(GetOwner()->HasAuthority());

	int32 MergeCount = 0;
	EInventoryOpResult Result = CanMoveOrMergeItem(FromSlotPos, ToSlotPos, MergeCount);
	if (Result == EInventoryOpResult::Fail)
		return;

	switch (Result)
	{
	case EInventoryOpResult::Move:  InventoryList.Unsafe_MoveItem(FromSlotPos, ToSlotPos);				break;
	case EInventoryOpResult::Merge: InventoryList.Unsafe_MergeItem(FromSlotPos, ToSlotPos, MergeCount); break;
	}
}

bool UD1InventoryManagerComponent::TryAddItem(const FIntPoint& ItemSlotPos, int32 ItemID, int32 ItemCount)
{
	return InventoryList.TryAddItem(ItemSlotPos, ItemID, ItemCount);
}

bool UD1InventoryManagerComponent::TryAddItem(int32 ItemID, int32 ItemCount)
{
	return InventoryList.TryAddItem(ItemID, ItemCount);
}

bool UD1InventoryManagerComponent::TryRemoveItem(const FIntPoint& ItemSlotPos, int32 ItemCount)
{
	return InventoryList.TryRemoveItem(ItemSlotPos, ItemCount);
}

bool UD1InventoryManagerComponent::TryRemoveItem(int32 ItemID, int32 ItemCount)
{
	return InventoryList.TryRemoveItem(ItemID, ItemCount);
}

const TArray<FD1InventoryEntry>& UD1InventoryManagerComponent::GetAllEntries() const
{
	return InventoryList.GetAllEntries();
}

FD1InventoryEntry UD1InventoryManagerComponent::GetEntryByPosition(const FIntPoint& ItemSlotPos)
{
	return InventoryList.GetEntryByPosition(ItemSlotPos);
}

bool UD1InventoryManagerComponent::IsEmpty(const TArray<TArray<bool>>& SlotChecks, const FIntPoint& ItemSlotPos, const FIntPoint& ItemSlotCount) const
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
			if (SlotChecks[y][x])
				return false;
		}
	}
	return true;
}

bool UD1InventoryManagerComponent::IsEmpty(const FIntPoint& ItemSlotPos, const FIntPoint& ItemSlotCount) const
{
	return IsEmpty(InventorySlotChecks, ItemSlotPos, ItemSlotCount);
}

EInventoryOpResult UD1InventoryManagerComponent::CanMoveOrMergeItem(const FIntPoint& FromSlotPos, const FIntPoint& ToSlotPos, int32& OutMergeCount) const
{
	OutMergeCount = 0;

	if (FromSlotPos.X < 0 || FromSlotPos.Y < 0 || FromSlotPos.X >= InventorySlotCount.X || FromSlotPos.Y >= InventorySlotCount.Y)
		return EInventoryOpResult::Fail;

	if (ToSlotPos.X < 0 || ToSlotPos.Y < 0 || ToSlotPos.X >= InventorySlotCount.X || ToSlotPos.Y >= InventorySlotCount.Y)
		return EInventoryOpResult::Fail;

	const TArray<FD1InventoryEntry>& Entries = GetAllEntries();

	int32 FromIndex = FromSlotPos.Y * InventorySlotCount.X + FromSlotPos.X;
	const FD1InventoryEntry& FromEntry = Entries[FromIndex];

	int32 ToIndex = ToSlotPos.Y * InventorySlotCount.X + ToSlotPos.X;
	const FD1InventoryEntry& ToEntry = Entries[ToIndex];

	if (const UD1ItemInstance* FromItemInstance = FromEntry.GetItemInstance())
	{
		if (FromSlotPos == ToSlotPos)
			return EInventoryOpResult::Move;

		const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
		const int32 FromItemID = FromItemInstance->GetItemID();
		const FD1ItemDefinition& FromItemDef = ItemData->GetItemDefByID(FromItemID);
		const FIntPoint& FromItemSlotCount = FromItemDef.ItemSlotCount;

		if (const UD1ItemInstance* ToItemInstance = ToEntry.GetItemInstance())
		{
			const int32 ToItemID = ToItemInstance->GetItemID();
			if (FromItemID != ToItemID)
				return EInventoryOpResult::Fail;

			const UD1ItemFragment_Stackable* Stackable = FromItemDef.FindFragmentByClass<UD1ItemFragment_Stackable>();
			if (Stackable == nullptr)
				return EInventoryOpResult::Fail;

			OutMergeCount = FMath::Min(FromEntry.GetItemCount() + ToEntry.GetItemCount(), Stackable->MaxStackCount) - ToEntry.GetItemCount();
			return OutMergeCount ? EInventoryOpResult::Merge : EInventoryOpResult::Fail;
		}
		else
		{
			if (ToSlotPos.X + FromItemSlotCount.X > InventorySlotCount.X || ToSlotPos.Y + FromItemSlotCount.Y > InventorySlotCount.Y)
				return EInventoryOpResult::Fail;

			TArray<TArray<bool>> TempSlotChecks = InventorySlotChecks;

			FIntPoint StartSlotPos = FromSlotPos;
			FIntPoint EndSlotPos = FromSlotPos + FromItemSlotCount;

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
						return EInventoryOpResult::Fail;
				}
			}
			return EInventoryOpResult::Move;
		}
	}
	return EInventoryOpResult::Fail;
}

void UD1InventoryManagerComponent::MarkSlotChecks(TArray<TArray<bool>>& SlotChecks, bool bIsUsing, const FIntPoint& ItemSlotPos, const FIntPoint& ItemSlotCount)
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
			SlotChecks[y][x] = bIsUsing;
		}
	}
}

void UD1InventoryManagerComponent::MarkSlotChecks(bool bIsUsing, const FIntPoint& ItemSlotPos, const FIntPoint& ItemSlotCount)
{
	MarkSlotChecks(InventorySlotChecks, bIsUsing, ItemSlotPos, ItemSlotCount);
}
