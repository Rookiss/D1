#include "D1InventoryManagerComponent.h"

#include "D1EquipmentManagerComponent.h"
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
		FD1InventoryEntry& Entry = Entries[Index];
		if (Entry.ItemInstance == nullptr)
			continue;

		const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(Entry.GetItemInstance()->GetItemID());
		const FIntPoint ItemSlotPos = FIntPoint(Index % InventorySlotCount.X, Index / InventorySlotCount.X);
		const FIntPoint& ItemSlotCount = ItemDef.ItemSlotCount;

		OwnerComponent->MarkSlotChecks(true, ItemSlotPos, ItemSlotCount);

		OwnerComponent->OnInventoryEntryChanged.Broadcast(ItemSlotPos, Entry.ItemInstance, Entry.LatestValidItemID, 0, Entry.ItemCount);
		Entry.LastObservedCount = Entry.ItemCount;
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
		FD1InventoryEntry& Entry = Entries[Index];
		if (Entry.ItemCount > 0)
		{
			AliveIndices.Add(Index);
			continue;
		}

		if (Entry.LatestValidItemID <= 0)
			continue;

		const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(Entry.LatestValidItemID);
		const FIntPoint ItemSlotPos = FIntPoint(Index % InventorySlotCount.X, Index / InventorySlotCount.X);
		const FIntPoint& ItemSlotCount = ItemDef.ItemSlotCount;

		OwnerComponent->MarkSlotChecks(false, ItemSlotPos, ItemSlotCount);

		OwnerComponent->OnInventoryEntryChanged.Broadcast(ItemSlotPos, Entry.ItemInstance, Entry.LatestValidItemID, Entry.LastObservedCount, Entry.ItemCount);
		Entry.LastObservedCount = Entry.ItemCount;
	}

	for (int32 Index : AliveIndices)
	{
		FD1InventoryEntry& Entry = Entries[Index];

		const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID((Entry.GetItemInstance()->GetItemID()));
		const FIntPoint ItemSlotPos = FIntPoint(Index % InventorySlotCount.X, Index / InventorySlotCount.X);
		const FIntPoint& ItemSlotCount = ItemDef.ItemSlotCount;

		OwnerComponent->MarkSlotChecks(true, ItemSlotPos, ItemSlotCount);
		
		OwnerComponent->OnInventoryEntryChanged.Broadcast(ItemSlotPos, Entry.ItemInstance, Entry.LatestValidItemID, Entry.LastObservedCount, Entry.ItemCount);
		Entry.LastObservedCount = Entry.ItemCount;
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

void FD1InventoryList::AddItem_Unsafe(const FIntPoint& ItemSlotPos, UD1ItemInstance* ItemInstance, int32 ItemCount)
{
	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(ItemInstance->ItemID);
	
	const FIntPoint& InventorySlotCount = OwnerComponent->GetInventorySlotCount();
	int32 Index = ItemSlotPos.Y * InventorySlotCount.X + ItemSlotPos.X;
	FD1InventoryEntry& Entry = Entries[Index];

	if (Entry.ItemInstance)
	{
		Entry.ItemCount += ItemCount;
		MarkItemDirty(Entry);
	}
	else
	{
		OwnerComponent->MarkSlotChecks(true, ItemSlotPos, ItemDef.ItemSlotCount);
		Entry.Init(ItemInstance, ItemCount);
		MarkItemDirty(Entry);
	}
}

UD1ItemInstance* FD1InventoryList::RemoveItem_Unsafe(const FIntPoint& ItemSlotPos, int32 ItemCount)
{
	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();

	const FIntPoint& InventorySlotCount = OwnerComponent->GetInventorySlotCount();
	const int32 Index = ItemSlotPos.Y * InventorySlotCount.X + ItemSlotPos.X;
	FD1InventoryEntry& Entry = Entries[Index];
	UD1ItemInstance* ItemInstance = Entry.ItemInstance;
	
	Entry.ItemCount -= ItemCount;
	if (Entry.ItemCount <= 0)
	{
		const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(ItemInstance->ItemID);
		OwnerComponent->MarkSlotChecks(false, ItemSlotPos, ItemDef.ItemSlotCount);
		Entry.Reset();
	}
	MarkItemDirty(Entry);
	return ItemInstance;
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

void UD1InventoryManagerComponent::Init(FIntPoint InInventorySlotCount)
{
	InventorySlotCount = InInventorySlotCount;
	
	SlotChecks.SetNumZeroed(InventorySlotCount.Y);
	for (int32 y = 0; y < SlotChecks.Num(); y++)
	{
		SlotChecks[y].SetNumZeroed(InventorySlotCount.X);
	}

	InventoryList.Entries.Reserve(InventorySlotCount.X * InventorySlotCount.Y);
}

void UD1InventoryManagerComponent::Server_RequestMoveOrMergeItem_FromInternalInventory_Implementation(const FIntPoint& FromItemSlotPos, const FIntPoint& ToItemSlotPos)
{
	check(GetOwner()->HasAuthority());
	
	int32 MovableItemCount = CanMoveOrMergeItem_FromInternalInventory(FromItemSlotPos, ToItemSlotPos);
	if (MovableItemCount == 0)
		return;

	UD1ItemInstance* RemovedItemInstance = InventoryList.RemoveItem_Unsafe(FromItemSlotPos, MovableItemCount);
	InventoryList.AddItem_Unsafe(ToItemSlotPos, RemovedItemInstance, MovableItemCount);
}

int32 UD1InventoryManagerComponent::CanMoveOrMergeItem_FromInternalInventory(const FIntPoint& FromItemSlotPos, const FIntPoint& ToItemSlotPos) const
{
	if (FromItemSlotPos.X < 0 || FromItemSlotPos.Y < 0 || FromItemSlotPos.X >= InventorySlotCount.X || FromItemSlotPos.Y >= InventorySlotCount.Y)
		return 0;

	if (ToItemSlotPos.X < 0 || ToItemSlotPos.Y < 0 || ToItemSlotPos.X >= InventorySlotCount.X || ToItemSlotPos.Y >= InventorySlotCount.Y)
		return 0;

	const TArray<FD1InventoryEntry>& Entries = GetAllEntries();

	int32 FromIndex = FromItemSlotPos.Y * InventorySlotCount.X + FromItemSlotPos.X;
	const FD1InventoryEntry& FromEntry = Entries[FromIndex];
	const int32 FromItemCount = FromEntry.ItemCount;

	int32 ToIndex = ToItemSlotPos.Y * InventorySlotCount.X + ToItemSlotPos.X;
	const FD1InventoryEntry& ToEntry = Entries[ToIndex];
	const int32 ToItemCount = ToEntry.ItemCount;
	
	const UD1ItemInstance* FromItemInstance = FromEntry.ItemInstance;
	if (FromItemInstance == nullptr)
		return 0;
	
	if (FromItemSlotPos == ToItemSlotPos)
		return FromItemCount;

	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	const int32 FromItemID = FromItemInstance->GetItemID();
	const FD1ItemDefinition& FromItemDef = ItemData->GetItemDefByID(FromItemID);
	const FIntPoint& FromItemSlotCount = FromItemDef.ItemSlotCount;

	if (const UD1ItemInstance* ToItemInstance = ToEntry.ItemInstance)
	{
		const int32 ToItemID = ToItemInstance->GetItemID();
		if (FromItemID != ToItemID)
			return 0;

		const UD1ItemFragment_Stackable* Stackable = FromItemDef.FindFragmentByClass<UD1ItemFragment_Stackable>();
		if (Stackable == nullptr)
			return 0;

		return FMath::Min(FromItemCount + ToItemCount, Stackable->MaxStackCount) - ToItemCount;
	}
	else
	{
		if (ToItemSlotPos.X + FromItemSlotCount.X > InventorySlotCount.X || ToItemSlotPos.Y + FromItemSlotCount.Y > InventorySlotCount.Y)
			return 0;

		TArray<TArray<bool>> TempSlotChecks = SlotChecks;

		FIntPoint StartSlotPos = FromItemSlotPos;
		FIntPoint EndSlotPos = FromItemSlotPos + FromItemSlotCount;

		for (int32 y = StartSlotPos.Y; y < EndSlotPos.Y; y++)
		{
			for (int32 x = StartSlotPos.X; x < EndSlotPos.X; x++)
			{
				TempSlotChecks[y][x] = false;
			}
		}

		StartSlotPos = ToItemSlotPos;
		EndSlotPos = ToItemSlotPos + FromItemSlotCount;

		for (int32 y = StartSlotPos.Y; y < EndSlotPos.Y; y++)
		{
			for (int32 x = StartSlotPos.X; x < EndSlotPos.X; x++)
			{
				if (TempSlotChecks[y][x])
					return 0;
			}
		}
		return FromItemCount;
	}
}

void UD1InventoryManagerComponent::Server_RequestMoveOrMergeItem_FromExternalInventory_Implementation(UD1InventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, const FIntPoint& ToItemSlotPos)
{
	check(GetOwner()->HasAuthority());
	
	int32 MovableItemCount = CanMoveOrMergeItem_FromExternalInventory(OtherComponent, FromItemSlotPos, ToItemSlotPos);
	if (MovableItemCount == 0)
		return;

	UD1ItemInstance* RemovedItemInstance = OtherComponent->InventoryList.RemoveItem_Unsafe(FromItemSlotPos, MovableItemCount);
	InventoryList.AddItem_Unsafe(ToItemSlotPos, RemovedItemInstance, MovableItemCount);
}

int32 UD1InventoryManagerComponent::CanMoveOrMergeItem_FromExternalInventory(UD1InventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, const FIntPoint& ToItemSlotPos) const
{
	if (OtherComponent == nullptr)
		return 0;

	const FIntPoint& FromInventorySlotCount = OtherComponent->GetInventorySlotCount();
	const FIntPoint& ToInventorySlotCount = InventorySlotCount;
	
	if (FromItemSlotPos.X < 0 || FromItemSlotPos.Y < 0 || FromItemSlotPos.X >= FromInventorySlotCount.X || FromItemSlotPos.Y >= FromInventorySlotCount.Y)
		return 0;
	
	if (ToItemSlotPos.X < 0 || ToItemSlotPos.Y < 0 || ToItemSlotPos.X >= ToInventorySlotCount.X || ToItemSlotPos.Y >= ToInventorySlotCount.Y)
		return 0;

	const TArray<FD1InventoryEntry>& FromEntries = OtherComponent->GetAllEntries();
	const TArray<FD1InventoryEntry>& ToEntries = GetAllEntries();

	const int32 FromIndex = FromItemSlotPos.Y * FromInventorySlotCount.X + FromItemSlotPos.X;
	const FD1InventoryEntry& FromEntry = FromEntries[FromIndex];
	const UD1ItemInstance* FromItemInstance = FromEntry.ItemInstance;
	const int32 FromItemCount = FromEntry.ItemCount;

	if (FromItemInstance == nullptr)
		return 0;
	
	const int32 ToIndex = ToItemSlotPos.Y * ToInventorySlotCount.X + ToItemSlotPos.X;
	const FD1InventoryEntry& ToEntry = ToEntries[ToIndex];
	const UD1ItemInstance* ToItemInstance = ToEntry.ItemInstance;

	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	const int32 FromItemID = FromItemInstance->GetItemID();
	
	const FD1ItemDefinition& FromItemDef = ItemData->GetItemDefByID(FromItemID);
	
	if (ToItemInstance)
	{
		const int32 ToItemID = ToItemInstance->GetItemID();
		if (FromItemID != ToItemID)
			return 0;

		const UD1ItemFragment_Stackable* Stackable = FromItemDef.FindFragmentByClass<UD1ItemFragment_Stackable>();
		if (Stackable == nullptr)
			return 0;

		return FMath::Min(FromItemCount + ToEntry.ItemCount, Stackable->MaxStackCount) - ToEntry.ItemCount;
	}
	else
	{
		const FIntPoint& FromItemSlotCount = FromItemDef.ItemSlotCount;
		return IsEmpty(ToItemSlotPos, FromItemSlotCount) ? FromItemCount : 0;
	}
}

void UD1InventoryManagerComponent::Server_RequestMoveOrMergeItem_FromExternalEquipment_Implementation(UD1EquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, const FIntPoint& ToItemSlotPos)
{
	check(GetOwner()->HasAuthority());
	
	if (CanMoveOrMergeItem_FromExternalEquipment(OtherComponent, FromEquipmentSlotType, ToItemSlotPos) == false)
		return;

	UD1ItemInstance* UnEquippedItemInstance = OtherComponent->EquipmentList.UnequipItem_Unsafe(FromEquipmentSlotType);
	InventoryList.AddItem_Unsafe(ToItemSlotPos, UnEquippedItemInstance, 1);
}

bool UD1InventoryManagerComponent::CanMoveOrMergeItem_FromExternalEquipment(UD1EquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, const FIntPoint& ToItemSlotPos) const
{
	if (OtherComponent == nullptr)
		return false;

	if (FromEquipmentSlotType == EEquipmentSlotType::EquipmentSlotCount)
		return false;
	
	if (ToItemSlotPos.X < 0 || ToItemSlotPos.Y < 0 || ToItemSlotPos.X >= InventorySlotCount.X || ToItemSlotPos.Y >= InventorySlotCount.Y)
		return false;

	const TArray<FD1EquipmentEntry>& FromEntries = OtherComponent->GetAllEntries();
	const TArray<FD1InventoryEntry>& ToEntries = GetAllEntries();
	
	const FD1EquipmentEntry& FromEntry = FromEntries[FromEquipmentSlotType];
	const UD1ItemInstance* FromItemInstance = FromEntry.GetItemInstance();

	if (FromItemInstance == nullptr)
		return false;
	
	int32 ToIndex = ToItemSlotPos.Y * InventorySlotCount.X + ToItemSlotPos.X;
	const FD1InventoryEntry& ToEntry = ToEntries[ToIndex];
	const UD1ItemInstance* ToItemInstance = ToEntry.ItemInstance;

	if (ToItemInstance)
		return false;

	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	const int32 FromItemID = FromItemInstance->GetItemID();
	const FD1ItemDefinition& FromItemDef = ItemData->GetItemDefByID(FromItemID);

	const FIntPoint& FromItemSlotCount = FromItemDef.ItemSlotCount;
	return IsEmpty(ToItemSlotPos, FromItemSlotCount);
}

bool UD1InventoryManagerComponent::TryAddItem(int32 ItemID, int32 ItemCount)
{
	return InventoryList.TryAddItem(ItemID, ItemCount);
}

bool UD1InventoryManagerComponent::TryRemoveItem(int32 ItemID, int32 ItemCount)
{
	return InventoryList.TryRemoveItem(ItemID, ItemCount);
}

const TArray<FD1InventoryEntry>& UD1InventoryManagerComponent::GetAllEntries() const
{
	return InventoryList.GetAllEntries();
}

int32 UD1InventoryManagerComponent::GetTotalCountByID(int32 ItemID) const
{
	return InventoryList.GetTotalCountByID(ItemID);
}

bool UD1InventoryManagerComponent::IsEmpty(const TArray<TArray<bool>>& InSlotChecks, const FIntPoint& ItemSlotPos, const FIntPoint& ItemSlotCount) const
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
			if (InSlotChecks[y][x])
				return false;
		}
	}
	return true;
}

bool UD1InventoryManagerComponent::IsEmpty(const FIntPoint& ItemSlotPos, const FIntPoint& ItemSlotCount) const
{
	return IsEmpty(SlotChecks, ItemSlotPos, ItemSlotCount);
}

void UD1InventoryManagerComponent::MarkSlotChecks(TArray<TArray<bool>>& InSlotChecks, bool bIsUsing, const FIntPoint& ItemSlotPos, const FIntPoint& ItemSlotCount)
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
			InSlotChecks[y][x] = bIsUsing;
		}
	}
}

void UD1InventoryManagerComponent::MarkSlotChecks(bool bIsUsing, const FIntPoint& ItemSlotPos, const FIntPoint& ItemSlotCount)
{
	MarkSlotChecks(SlotChecks, bIsUsing, ItemSlotPos, ItemSlotCount);
}
