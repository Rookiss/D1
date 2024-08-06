#include "D1InventoryManagerComponent.h"

#include "D1EquipmentManagerComponent.h"
#include "D1ItemManagerComponent.h"
#include "Data/D1ItemData.h"
#include "Engine/ActorChannel.h"
#include "Item/D1ItemInstance.h"
#include "Item/D1ItemTemplate.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1InventoryManagerComponent)

UD1ItemInstance* FD1InventoryEntry::Init(int32 InItemTemplateID, int32 InItemCount, EItemRarity InItemRarity)
{
	check(InItemTemplateID > 0 && InItemCount > 0 && InItemRarity != EItemRarity::Count);
	
	UD1ItemInstance* NewItemInstance = NewObject<UD1ItemInstance>();
	NewItemInstance->Init(InItemTemplateID, InItemRarity);
	Init(NewItemInstance, InItemCount);
	
	return NewItemInstance;
}

void FD1InventoryEntry::Init(UD1ItemInstance* InItemInstance, int32 InItemCount)
{
	check(InItemInstance && InItemCount > 0);
	
	ItemInstance = InItemInstance;
	
	const UD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());
	ItemCount = FMath::Clamp(InItemCount, 1, ItemTemplate.MaxStackCount);
}

UD1ItemInstance* FD1InventoryEntry::Reset()
{
	UD1ItemInstance* RemovedItemInstance = ItemInstance;
	
	ItemInstance = nullptr;
	ItemCount = 0;

	return RemovedItemInstance;
}

bool FD1InventoryList::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
{
	return FFastArraySerializer::FastArrayDeltaSerialize<FD1InventoryEntry, FD1InventoryList>(Entries, DeltaParams, *this);
}

void FD1InventoryList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	const FIntPoint& InventorySlotCount = InventoryManager->GetInventorySlotCount();

	for (int32 AddedIndex : AddedIndices)
	{
		FD1InventoryEntry& Entry = Entries[AddedIndex];
		if (Entry.ItemInstance)
		{
			const FIntPoint ItemSlotPos = FIntPoint(AddedIndex % InventorySlotCount.X, AddedIndex / InventorySlotCount.X);
			BroadcastChangedMessage(ItemSlotPos, Entry.ItemInstance, Entry.ItemCount);
		}
	}
}

void FD1InventoryList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	TArray<int32> AddedIndices;
	AddedIndices.Reserve(FinalSize);

	const FIntPoint& InventorySlotCount = InventoryManager->GetInventorySlotCount();
	
	for (int32 ChangedIndex : ChangedIndices)
	{
		FD1InventoryEntry& Entry = Entries[ChangedIndex];
		if (Entry.ItemInstance)
		{
			AddedIndices.Add(ChangedIndex);
		}
		else
		{
			const FIntPoint ItemSlotPos = FIntPoint(ChangedIndex % InventorySlotCount.X, ChangedIndex / InventorySlotCount.X);
			BroadcastChangedMessage(ItemSlotPos, nullptr, 0);
		}
	}

	for (int32 AddedIndex : AddedIndices)
	{
		FD1InventoryEntry& Entry = Entries[AddedIndex];
		const FIntPoint ItemSlotPos = FIntPoint(AddedIndex % InventorySlotCount.X, AddedIndex / InventorySlotCount.X);
		BroadcastChangedMessage(ItemSlotPos, Entry.ItemInstance, Entry.ItemCount);
	}
}

void FD1InventoryList::BroadcastChangedMessage(const FIntPoint& ItemSlotPos, UD1ItemInstance* ItemInstance, int32 ItemCount)
{
	if (InventoryManager->OnInventoryEntryChanged.IsBound())
	{
		InventoryManager->OnInventoryEntryChanged.Broadcast(ItemSlotPos, ItemInstance, ItemCount);
	}
}

UD1InventoryManagerComponent::UD1InventoryManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, InventoryList(this)
{
	bWantsInitializeComponent = true;
	SetIsReplicatedByDefault(true);
}

void UD1InventoryManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		TArray<FD1InventoryEntry>& Entries = InventoryList.Entries;
		Entries.SetNum(InventorySlotCount.X * InventorySlotCount.Y);
	
		for (FD1InventoryEntry& Entry : Entries)
		{
			InventoryList.MarkItemDirty(Entry);
		}

		SlotChecks.SetNumZeroed(InventorySlotCount.X * InventorySlotCount.Y);
	}
}

void UD1InventoryManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, InventoryList);
	DOREPLIFETIME(ThisClass, SlotChecks);
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

void UD1InventoryManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();
	
	if (IsUsingRegisteredSubObjectList())
	{
		for (const FD1InventoryEntry& Entry : InventoryList.Entries)
		{
			UD1ItemInstance* Instance = Entry.GetItemInstance();
			if (IsValid(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}
	}
}

int32 UD1InventoryManagerComponent::CanMoveOrMergeItem(UD1InventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, const FIntPoint& ToItemSlotPos) const
{
	if (OtherComponent == nullptr)
		return 0;

	const FIntPoint& FromInventorySlotCount = OtherComponent->GetInventorySlotCount();
	const FIntPoint& ToInventorySlotCount = InventorySlotCount;
	
	if (FromItemSlotPos.X < 0 || FromItemSlotPos.Y < 0 || FromItemSlotPos.X >= FromInventorySlotCount.X || FromItemSlotPos.Y >= FromInventorySlotCount.Y)
		return 0;
	
	if (ToItemSlotPos.X < 0 || ToItemSlotPos.Y < 0 || ToItemSlotPos.X >= ToInventorySlotCount.X || ToItemSlotPos.Y >= ToInventorySlotCount.Y)
		return 0;
	
	const UD1ItemInstance* FromItemInstance = OtherComponent->GetItemInstance(FromItemSlotPos);
	const int32 FromItemCount = OtherComponent->GetItemCount(FromItemSlotPos);

	const UD1ItemInstance* ToItemInstance = GetItemInstance(ToItemSlotPos);
	const int32 ToItemCount = GetItemCount(ToItemSlotPos);
	
	if (FromItemInstance == nullptr)
		return 0;

	if (this == OtherComponent && FromItemSlotPos == ToItemSlotPos)
		return FromItemCount;
	
	const int32 FromTemplateID = FromItemInstance->GetItemTemplateID();
	const UD1ItemTemplate& FromItemTemplate = UD1ItemData::Get().FindItemTemplateByID(FromTemplateID);
	
	if (ToItemInstance)
	{
		const int32 ToTemplateID = ToItemInstance->GetItemTemplateID();
		if (FromTemplateID != ToTemplateID)
			return 0;

		if (FromItemInstance->GetItemRarity() != ToItemInstance->GetItemRarity())
			return 0;
		
		if (FromItemTemplate.MaxStackCount < 2)
			return 0;

		return FMath::Min(FromItemCount + ToItemCount, FromItemTemplate.MaxStackCount) - ToItemCount;
	}
	else
	{
		const FIntPoint& FromItemSlotCount = FromItemTemplate.SlotCount;
		if (ToItemSlotPos.X + FromItemSlotCount.X > InventorySlotCount.X || ToItemSlotPos.Y + FromItemSlotCount.Y > InventorySlotCount.Y)
			return 0;

		if (this == OtherComponent)
		{
			TArray<bool> TempSlotChecks = SlotChecks;
			MarkSlotChecks(TempSlotChecks, false, FromItemSlotPos, FromItemSlotCount);
			
			return IsEmpty(TempSlotChecks, ToItemSlotPos, FromItemSlotCount) ? FromItemCount : 0;
		}
		else
		{
			return IsEmpty(ToItemSlotPos, FromItemSlotCount) ? FromItemCount : 0;
		}
	}
}

int32 UD1InventoryManagerComponent::CanMoveOrMergeItem(UD1EquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, const FIntPoint& ToItemSlotPos) const
{
	if (OtherComponent == nullptr)
		return 0;

	if (FromEquipmentSlotType == EEquipmentSlotType::Unarmed_LeftHand || FromEquipmentSlotType == EEquipmentSlotType::Unarmed_RightHand || FromEquipmentSlotType == EEquipmentSlotType::Count)
		return 0;
	
	if (ToItemSlotPos.X < 0 || ToItemSlotPos.Y < 0 || ToItemSlotPos.X >= InventorySlotCount.X || ToItemSlotPos.Y >= InventorySlotCount.Y)
		return 0;
	
	const UD1ItemInstance* FromItemInstance = OtherComponent->GetItemInstance(FromEquipmentSlotType);
	const int32 FromItemCount = OtherComponent->GetItemCount(FromEquipmentSlotType);
	
	return CanMoveOrMergeItem(FromItemInstance, FromItemCount, ToItemSlotPos);
}

int32 UD1InventoryManagerComponent::CanMoveOrMergeItem_Quick(UD1InventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, TArray<FIntPoint>& OutToItemSlotPoses, TArray<int32>& OutToItemCounts) const
{
	if (OtherComponent == nullptr || this == OtherComponent)
		return 0;

	const FIntPoint& FromInventorySlotCount = OtherComponent->GetInventorySlotCount();
	if (FromItemSlotPos.X < 0 || FromItemSlotPos.Y < 0 || FromItemSlotPos.X >= FromInventorySlotCount.X || FromItemSlotPos.Y >= FromInventorySlotCount.Y)
		return 0;

	OutToItemSlotPoses.Reset();
	OutToItemCounts.Reset();
	
	const TArray<FD1InventoryEntry>& FromEntries = OtherComponent->GetAllEntries();
	const int32 FromIndex = FromItemSlotPos.Y * FromInventorySlotCount.X + FromItemSlotPos.X;
	const FD1InventoryEntry& FromEntry = FromEntries[FromIndex];
	const UD1ItemInstance* FromItemInstance = FromEntry.ItemInstance;
	const int32 FromItemCount = FromEntry.ItemCount;
	int32 LeftItemCount = FromEntry.ItemCount;

	if (FromItemInstance == nullptr)
		return 0;

	const int32 FromItemTemplateID = FromItemInstance->GetItemTemplateID();
	const UD1ItemTemplate& FromItemTemplate = UD1ItemData::Get().FindItemTemplateByID(FromItemTemplateID);
	
	if (FromItemTemplate.MaxStackCount > 1)
	{
		const TArray<FD1InventoryEntry>& ToEntries = GetAllEntries();
		for (int32 i = 0; i < ToEntries.Num(); i++)
		{
			const FD1InventoryEntry& Entry = ToEntries[i];
			
			if (Entry.ItemInstance == nullptr)
				continue;
			
			if (Entry.ItemInstance->GetItemTemplateID() != FromItemTemplateID)
				continue;
			
			if (Entry.ItemInstance->GetItemRarity() != FromItemInstance->GetItemRarity())
				continue;
			
			if (int32 AddCount = FMath::Min(Entry.GetItemCount() + LeftItemCount, FromItemTemplate.MaxStackCount) - Entry.GetItemCount())
			{
				OutToItemSlotPoses.Emplace(i % InventorySlotCount.X, i / InventorySlotCount.X);
				OutToItemCounts.Emplace(AddCount);
				LeftItemCount -= AddCount;
			
				if (LeftItemCount == 0)
					return FromItemCount;
			}
		}
	}

	const FIntPoint& FromItemSlotCount = FromItemTemplate.SlotCount;
	TArray<bool> TempSlotChecks = SlotChecks;
	
	const FIntPoint StartSlotPos = FIntPoint::ZeroValue;
	const FIntPoint EndSlotPos = InventorySlotCount - FromItemSlotCount;
	
	for (int32 y = StartSlotPos.Y; y <= EndSlotPos.Y; y++)
	{
		for (int32 x = StartSlotPos.X; x <= EndSlotPos.X; x++)
		{
			int32 Index = y * InventorySlotCount.X + x;
			if (TempSlotChecks.IsValidIndex(Index) == false || TempSlotChecks[Index])
				continue;

			FIntPoint ItemSlotPos = FIntPoint(x, y);
			if (IsEmpty(TempSlotChecks, ItemSlotPos, FromItemSlotCount))
			{
				MarkSlotChecks(TempSlotChecks, true, ItemSlotPos, FromItemSlotCount);
				
				int32 AddCount = FMath::Min(LeftItemCount, FromItemTemplate.MaxStackCount);
				OutToItemSlotPoses.Emplace(x, y);
				OutToItemCounts.Emplace(AddCount);
				LeftItemCount -= AddCount;
				
				if (LeftItemCount == 0)
					return FromItemCount;
			}
		}
	}
	return FromItemCount - LeftItemCount;
}

bool UD1InventoryManagerComponent::CanMoveOrMergeItem_Quick(UD1EquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, FIntPoint& OutToItemSlotPos) const
{
	if (OtherComponent == nullptr)
		return false;

	if (FromEquipmentSlotType == EEquipmentSlotType::Unarmed_LeftHand || FromEquipmentSlotType == EEquipmentSlotType::Unarmed_RightHand || FromEquipmentSlotType == EEquipmentSlotType::Count)
		return false;

	const TArray<FD1EquipmentEntry>& FromEntries = OtherComponent->GetAllEntries();
	const FD1EquipmentEntry& FromEntry = FromEntries[(int32)FromEquipmentSlotType];
	const UD1ItemInstance* FromItemInstance = FromEntry.GetItemInstance();

	if (FromItemInstance == nullptr)
		return false;

	const int32 FromTemplateID = FromItemInstance->GetItemTemplateID();
	const UD1ItemTemplate& FromItemTemplate = UD1ItemData::Get().FindItemTemplateByID(FromTemplateID);
	
	const FIntPoint& ItemSlotCount = FromItemTemplate.SlotCount;

	const FIntPoint StartSlotPos = FIntPoint::ZeroValue;
	const FIntPoint EndSlotPos = GetInventorySlotCount() - ItemSlotCount;
	
	for (int32 y = StartSlotPos.Y; y <= EndSlotPos.Y; y++)
	{
		for (int32 x = StartSlotPos.X; x <= EndSlotPos.X; x++)
		{
			int32 Index = y * InventorySlotCount.X + x;
			if (SlotChecks.IsValidIndex(Index) == false || SlotChecks[Index])
				continue;

			FIntPoint ItemSlotPos = FIntPoint(x, y);
			if (IsEmpty(SlotChecks, ItemSlotPos, ItemSlotCount))
			{
				OutToItemSlotPos = ItemSlotPos;
				return true;
			}
		}
	}
	
	return false;
}

bool UD1InventoryManagerComponent::CanAddItem(int32 ItemTemplateID, EItemRarity ItemRarity, int32 ItemCount, TArray<FIntPoint>& OutToItemSlotPoses, TArray<int32>& OutToItemCounts) const
{
	OutToItemSlotPoses.Reset();
	OutToItemCounts.Reset();
	
	if (ItemTemplateID <= 0 || ItemCount <= 0)
		return false;
	
	const UD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(ItemTemplateID);
	const TArray<FD1InventoryEntry>& Entries = GetAllEntries();

	TArray<FIntPoint> ToItemSlotPoses;
	TArray<int32> ToItemCounts;
	
	if (ItemTemplate.MaxStackCount > 1)
	{
		for (int32 i = 0; i < Entries.Num(); i++)
		{
			const FD1InventoryEntry& Entry = Entries[i];
			
			if (Entry.ItemInstance == nullptr)
				continue;

			if (Entry.ItemInstance->GetItemTemplateID() != ItemTemplateID)
				continue;

			if (Entry.ItemInstance->GetItemRarity() != ItemRarity)
				continue;
			
			if (int32 AddCount = FMath::Min(Entry.GetItemCount() + ItemCount, ItemTemplate.MaxStackCount) - Entry.GetItemCount())
			{
				ToItemSlotPoses.Emplace(i % InventorySlotCount.X, i / InventorySlotCount.X);
				ToItemCounts.Emplace(AddCount);
				ItemCount -= AddCount;

				if (ItemCount == 0)
				{
					OutToItemSlotPoses = ToItemSlotPoses;
					OutToItemCounts = ToItemCounts;
					return true;
				}
			}
		}
	}
	
	const FIntPoint& ItemSlotCount = ItemTemplate.SlotCount;
	TArray<bool> TempSlotChecks = SlotChecks;
	
	const FIntPoint StartSlotPos = FIntPoint::ZeroValue;
	const FIntPoint EndSlotPos = InventorySlotCount - ItemSlotCount;
	
	for (int32 y = StartSlotPos.Y; y <= EndSlotPos.Y; y++)
	{
		for (int32 x = StartSlotPos.X; x <= EndSlotPos.X; x++)
		{
			int32 Index = y * InventorySlotCount.X + x;
			if (TempSlotChecks.IsValidIndex(Index) == false || TempSlotChecks[Index])
				continue;

			FIntPoint ItemSlotPos = FIntPoint(x, y);
			if (IsEmpty(TempSlotChecks, ItemSlotPos, ItemSlotCount))
			{
				MarkSlotChecks(TempSlotChecks, true, ItemSlotPos, ItemSlotCount);
				
				int32 AddCount = FMath::Min(ItemCount, ItemTemplate.MaxStackCount);
				ToItemSlotPoses.Emplace(ItemSlotPos);
				ToItemCounts.Emplace(AddCount);
				
				ItemCount -= AddCount;
				
				if (ItemCount == 0)
				{
					OutToItemSlotPoses = ToItemSlotPoses;
					OutToItemCounts = ToItemCounts;
					return true;
				}
			}
		}
	}
	
	return false;
}

bool UD1InventoryManagerComponent::CanRemoveItem(int32 ItemTemplateID, int32 ItemCount, TArray<FIntPoint>& OutToItemSlotPoses, TArray<int32>& OutToItemCounts) const
{
	OutToItemSlotPoses.Reset();
	OutToItemCounts.Reset();

	if (ItemTemplateID <= 0 || ItemCount <= 0)
		return false;
	
	TArray<FIntPoint> ToItemSlotPoses;
	TArray<int32> ToItemCounts;

	const TArray<FD1InventoryEntry>& Entries = GetAllEntries();
	
	for (int32 i = Entries.Num() - 1; i >= 0; i--)
	{
		const FD1InventoryEntry& ToEntry = Entries[i];
		if (ToEntry.ItemInstance == nullptr)
			continue;

		if (ToEntry.ItemInstance->GetItemTemplateID() != ItemTemplateID)
			continue;
		
		if (ToEntry.ItemCount < ItemCount)
		{
			ToItemSlotPoses.Emplace(i % InventorySlotCount.X, i / InventorySlotCount.X);
			ToItemCounts.Emplace(ToEntry.ItemCount);
			
			ItemCount -= ToEntry.ItemCount;
		}
		else
		{
			ToItemSlotPoses.Emplace(i % InventorySlotCount.X, i / InventorySlotCount.X);
			ToItemCounts.Emplace(ItemCount);

			OutToItemSlotPoses = ToItemSlotPoses;
			OutToItemCounts = ToItemCounts;
			return true;
		}
	}
	
	return false;
}

int32 UD1InventoryManagerComponent::CanMoveOrMergeItem(const UD1ItemInstance* FromItemInstance, int32 FromItemCount, const FIntPoint& ToItemSlotPos) const
{
	const UD1ItemInstance* ToItemInstance = GetItemInstance(ToItemSlotPos);
	const int32 ToItemCount = GetItemCount(ToItemSlotPos);

	if (FromItemInstance == nullptr)
		return 0;

	const int32 FromTemplateID = FromItemInstance->GetItemTemplateID();
	const UD1ItemTemplate& FromItemTemplate = UD1ItemData::Get().FindItemTemplateByID(FromTemplateID);
	
	if (ToItemInstance)
	{
		const int32 ToTemplateID = ToItemInstance->GetItemTemplateID();
		if (FromTemplateID != ToTemplateID)
			return 0;

		if (FromItemInstance->GetItemRarity() != ToItemInstance->GetItemRarity())
			return 0;
		
		if (FromItemTemplate.MaxStackCount < 2)
			return 0;

		return FMath::Min(FromItemCount + ToItemCount, FromItemTemplate.MaxStackCount) - ToItemCount;
	}
	else
	{
		const FIntPoint& FromItemSlotCount = FromItemTemplate.SlotCount;
		if (ToItemSlotPos.X + FromItemSlotCount.X > InventorySlotCount.X || ToItemSlotPos.Y + FromItemSlotCount.Y > InventorySlotCount.Y)
			return 0;

		return IsEmpty(ToItemSlotPos, FromItemSlotCount) ? FromItemCount : 0;
	}
}

bool UD1InventoryManagerComponent::TryAddItemByRarity(TSubclassOf<UD1ItemTemplate> ItemTemplateClass, EItemRarity ItemRarity, int32 ItemCount)
{
	check(GetOwner()->HasAuthority());

	int32 ItemTemplateID = UD1ItemData::Get().FindItemTemplateIDByClass(ItemTemplateClass);
	const UD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(ItemTemplateID);
	
	TArray<FIntPoint> ToItemSlotPoses;
	TArray<int32> ToItemCounts;
	
	if (CanAddItem(ItemTemplateID, ItemRarity, ItemCount, ToItemSlotPoses, ToItemCounts))
	{
		TArray<UD1ItemInstance*> AddedItemInstances;
		
		for (int32 i = 0; i < ToItemSlotPoses.Num(); i++)
		{
			const FIntPoint& ToItemSlotPos = ToItemSlotPoses[i];
			const int32 ToItemCount = ToItemCounts[i];

			const int32 ToIndex = ToItemSlotPos.Y * InventorySlotCount.X + ToItemSlotPos.X;
			FD1InventoryEntry& ToEntry = InventoryList.Entries[ToIndex];

			if (ToEntry.ItemInstance)
			{
				ToEntry.ItemCount += ToItemCount;
				InventoryList.MarkItemDirty(ToEntry);
			}
			else
			{
				AddedItemInstances.Add(ToEntry.Init(ItemTemplateID, ToItemCount, ItemRarity));
				MarkSlotChecks(true, ToItemSlotPos, ItemTemplate.SlotCount);
				InventoryList.MarkItemDirty(ToEntry);
			}
		}

		if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
		{
			for (UD1ItemInstance* AddedItemInstance : AddedItemInstances)
			{
				if (AddedItemInstance)
				{
					AddReplicatedSubObject(AddedItemInstance);
				}
			}
		}
		return true;
	}

	return false;
}

bool UD1InventoryManagerComponent::TryAddItemByProbability(TSubclassOf<UD1ItemTemplate> ItemTemplateClass, int32 ItemCount, const TArray<FD1ItemRarityProbability>& ItemProbabilities)
{
	check(GetOwner()->HasAuthority());

	return TryAddItemByRarity(ItemTemplateClass, UD1ItemInstance::DetermineItemRarity(ItemProbabilities), ItemCount);
}

bool UD1InventoryManagerComponent::TryRemoveItem(int32 ItemTemplateID, int32 ItemCount)
{
	check(GetOwner()->HasAuthority());
	
	const UD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(ItemTemplateID);

	TArray<FIntPoint> ToItemSlotPoses;
	TArray<int32> ToItemCounts;
	
	if (CanRemoveItem(ItemTemplateID, ItemCount, ToItemSlotPoses, ToItemCounts))
	{
		TArray<UD1ItemInstance*> RemovedItemInstances;

		for (int32 i = 0; i < ToItemSlotPoses.Num(); i++)
		{
			const FIntPoint& ToItemSlotPos = ToItemSlotPoses[i];
			const int32 ToItemCount = ToItemCounts[i];

			const int32 ToIndex = ToItemSlotPos.Y * InventorySlotCount.X + ToItemSlotPos.X;
			FD1InventoryEntry& ToEntry = InventoryList.Entries[ToIndex];

			ToEntry.ItemCount -= ToItemCount;
			
			if (ToEntry.ItemCount <= 0)
			{
				MarkSlotChecks(false, ToItemSlotPos, ItemTemplate.SlotCount);
				RemovedItemInstances.Add(ToEntry.Reset());
			}

			InventoryList.MarkItemDirty(ToEntry);
		}

		if (IsUsingRegisteredSubObjectList())
		{
			for (UD1ItemInstance* RemovedItemInstance : RemovedItemInstances)
			{
				if (RemovedItemInstance)
				{
					RemoveReplicatedSubObject(RemovedItemInstance);
				}
			}
		}
		return true;
	}

	return false;
}

void UD1InventoryManagerComponent::AddItem_Unsafe(const FIntPoint& ItemSlotPos, UD1ItemInstance* ItemInstance, int32 ItemCount)
{
	check(GetOwner()->HasAuthority());

	if (ItemInstance == nullptr)
		return;

	const int32 EntryIndex = ItemSlotPos.Y * InventorySlotCount.X + ItemSlotPos.X;
	FD1InventoryEntry& Entry = InventoryList.Entries[EntryIndex];

	if (Entry.ItemInstance)
	{
		Entry.ItemCount += ItemCount;
		InventoryList.MarkItemDirty(Entry);
	}
	else
	{
		UD1ItemInstance* AddedItemInstance;
		const UD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());
		
		AddedItemInstance = DuplicateObject<UD1ItemInstance>(ItemInstance, GetTransientPackage());
		
		Entry.Init(AddedItemInstance, ItemCount);

		if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && AddedItemInstance)
		{
			AddReplicatedSubObject(AddedItemInstance);
		}

		MarkSlotChecks(true, ItemSlotPos, ItemTemplate.SlotCount);
		InventoryList.MarkItemDirty(Entry);
	}
}

UD1ItemInstance* UD1InventoryManagerComponent::RemoveItem_Unsafe(const FIntPoint& ItemSlotPos, int32 ItemCount)
{
	check(GetOwner()->HasAuthority());
	
	const int32 EntryIndex = ItemSlotPos.Y * InventorySlotCount.X + ItemSlotPos.X;
	FD1InventoryEntry& Entry = InventoryList.Entries[EntryIndex];
	UD1ItemInstance* ItemInstance = Entry.ItemInstance;
	
	Entry.ItemCount -= ItemCount;
	if (Entry.ItemCount <= 0)
	{
		const UD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());
		MarkSlotChecks(false, ItemSlotPos, ItemTemplate.SlotCount);
		
		UD1ItemInstance* RemovedItemInstance = Entry.Reset();
		if (IsUsingRegisteredSubObjectList() && RemovedItemInstance)
		{
			RemoveReplicatedSubObject(RemovedItemInstance);
		}
	}
	
	InventoryList.MarkItemDirty(Entry);
	return ItemInstance;
}

void UD1InventoryManagerComponent::MarkSlotChecks(TArray<bool>& InSlotChecks, bool bIsUsing, const FIntPoint& ItemSlotPos, const FIntPoint& ItemSlotCount) const
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
			int32 Index = y * InventorySlotCount.X + x;
			if (InSlotChecks.IsValidIndex(Index))
			{
				InSlotChecks[Index] = bIsUsing;
			}
		}
	}
}

void UD1InventoryManagerComponent::MarkSlotChecks(bool bIsUsing, const FIntPoint& ItemSlotPos, const FIntPoint& ItemSlotCount)
{
	MarkSlotChecks(SlotChecks, bIsUsing, ItemSlotPos, ItemSlotCount);
}

bool UD1InventoryManagerComponent::IsEmpty(const TArray<bool>& InSlotChecks, const FIntPoint& ItemSlotPos, const FIntPoint& ItemSlotCount) const
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
			int32 Index = y * InventorySlotCount.X + x;
			if (InSlotChecks.IsValidIndex(Index) == false || InSlotChecks[Index])
				return false;
		}
	}
	return true;
}

bool UD1InventoryManagerComponent::IsEmpty(const FIntPoint& ItemSlotPos, const FIntPoint& ItemSlotCount) const
{
	return IsEmpty(SlotChecks, ItemSlotPos, ItemSlotCount);
}

bool UD1InventoryManagerComponent::IsAllEmpty()
{
	for (FD1InventoryEntry& Entry : InventoryList.Entries)
	{
		if (Entry.GetItemInstance())
			return false;
	}
	return true;
}

UD1ItemInstance* UD1InventoryManagerComponent::GetItemInstance(const FIntPoint& ItemSlotPos) const
{
	if (ItemSlotPos.X < 0 || ItemSlotPos.Y < 0 || ItemSlotPos.X >= InventorySlotCount.X || ItemSlotPos.Y >= InventorySlotCount.Y)
		return nullptr;
	
	const TArray<FD1InventoryEntry>& Entries = InventoryList.GetAllEntries();
	const int32 EntryIndex = ItemSlotPos.Y * InventorySlotCount.X + ItemSlotPos.X;
	const FD1InventoryEntry& Entry = Entries[EntryIndex];
	
	return Entry.GetItemInstance();
}

int32 UD1InventoryManagerComponent::GetItemCount(const FIntPoint& ItemSlotPos) const
{
	if (ItemSlotPos.X < 0 || ItemSlotPos.Y < 0 || ItemSlotPos.X >= InventorySlotCount.X || ItemSlotPos.Y >= InventorySlotCount.Y)
		return 0;
	
	const TArray<FD1InventoryEntry>& Entries = InventoryList.GetAllEntries();
	const int32 EntryIndex = ItemSlotPos.Y * InventorySlotCount.X + ItemSlotPos.X;
	const FD1InventoryEntry& Entry = Entries[EntryIndex];
	
	return Entry.GetItemCount();
}

const TArray<FD1InventoryEntry>& UD1InventoryManagerComponent::GetAllEntries() const
{
	return InventoryList.GetAllEntries();
}

int32 UD1InventoryManagerComponent::GetTotalCountByID(int32 ItemTemplateID) const
{
	int32 TotalCount = 0;
	
	for (const FD1InventoryEntry& Entry : GetAllEntries())
	{
		if (UD1ItemInstance* ItemInstance = Entry.ItemInstance)
		{
			if (ItemInstance->GetItemTemplateID() == ItemTemplateID)
			{
				TotalCount += Entry.ItemCount;
			}
		}
	}
	
	return TotalCount;
}
