#include "D1InventoryManagerComponent.h"

#include "D1EquipmentManagerComponent.h"
#include "D1GameplayTags.h"
#include "Data/D1ItemData.h"
#include "Engine/ActorChannel.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Item/D1ItemInstance.h"
#include "Item/Fragments/D1ItemFragment_Consumable.h"
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
	
	const UD1ItemFragment_Consumable* Consumable = ItemInstance->FindFragmentByClass<UD1ItemFragment_Consumable>();
	ItemCount = Consumable ? FMath::Clamp(InItemCount, 1, Consumable->MaxStackCount) : 1;
	
	LastValidTemplateID = ItemInstance->GetItemTemplateID();
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
		if (Entry.ItemInstance == nullptr)
			continue;

		const int32 ItemTemplateID = Entry.ItemInstance->GetItemTemplateID();
		const UD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(ItemTemplateID);
		const FIntPoint ItemSlotPos = FIntPoint(AddedIndex % InventorySlotCount.X, AddedIndex / InventorySlotCount.X);
		const FIntPoint& ItemSlotCount = ItemTemplate.SlotCount;

		InventoryManager->MarkSlotChecks(true, ItemSlotPos, ItemSlotCount);
		BroadcastChangedMessage(ItemSlotPos, Entry.ItemInstance, Entry.ItemCount);
	}
}

void FD1InventoryList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	const FIntPoint& InventorySlotCount = InventoryManager->GetInventorySlotCount();

	TArray<int32> AddedIndices;
	AddedIndices.Reserve(FinalSize);

	for (int32 ChangedIndex : ChangedIndices)
	{
		FD1InventoryEntry& Entry = Entries[ChangedIndex];
		if (Entry.ItemInstance)
		{
			AddedIndices.Add(ChangedIndex);
		}
		else
		{
			if (Entry.LastValidTemplateID > 0)
			{
				const UD1ItemTemplate& ItemTemplate =  UD1ItemData::Get().FindItemTemplateByID(Entry.LastValidTemplateID);
				const FIntPoint ItemSlotPos = FIntPoint(ChangedIndex % InventorySlotCount.X, ChangedIndex / InventorySlotCount.X);
				const FIntPoint& ItemSlotCount = ItemTemplate.SlotCount;

				InventoryManager->MarkSlotChecks(false, ItemSlotPos, ItemSlotCount);
				BroadcastChangedMessage(ItemSlotPos, Entry.ItemInstance, Entry.ItemCount);
			}
		}
	}

	for (int32 AddedIndex : AddedIndices)
	{
		FD1InventoryEntry& Entry = Entries[AddedIndex];

		const int32 ItemTemplateID = Entry.ItemInstance->GetItemTemplateID();
		const UD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(ItemTemplateID);
		const FIntPoint ItemSlotPos = FIntPoint(AddedIndex % InventorySlotCount.X, AddedIndex / InventorySlotCount.X);
		const FIntPoint& ItemSlotCount = ItemTemplate.SlotCount;

		InventoryManager->MarkSlotChecks(true, ItemSlotPos, ItemSlotCount);
		BroadcastChangedMessage(ItemSlotPos, Entry.ItemInstance, Entry.ItemCount);
	}
}

TArray<UD1ItemInstance*> FD1InventoryList::TryAddItem(int32 ItemTemplateID, int32 ItemCount, EItemRarity ItemRarity)
{
	TArray<UD1ItemInstance*> AddedItemInstances;
	 
	if (ItemTemplateID <= 0 || ItemCount <= 0 || ItemRarity == EItemRarity::Count)
		return AddedItemInstances;
	
	TArray<TPair<int32/*EntryIndex*/, int32/*AddCount*/>> AddQueue;
	TArray<TPair<int32/*EntryIndex*/, int32/*NewCount*/>> NewQueue;
	
	const UD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(ItemTemplateID);
	const UD1ItemFragment_Consumable* ConsumableFragment = ItemTemplate.FindFragmentByClass<UD1ItemFragment_Consumable>();
	
	if (ConsumableFragment)
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
			
			if (int32 AddCount = FMath::Min(Entry.GetItemCount() + ItemCount, ConsumableFragment->MaxStackCount) - Entry.GetItemCount())
			{
				AddQueue.Emplace(i, AddCount);
				ItemCount -= AddCount;

				if (ItemCount == 0)
				{
					for (const auto& Pair : AddQueue)
					{
						FD1InventoryEntry& ToEntry = Entries[Pair.Key];
						ToEntry.ItemCount += Pair.Value;
						MarkItemDirty(ToEntry);
					}
					return AddedItemInstances;
				}
			}
		}
	}
	
	const FIntPoint& ItemSlotCount = ItemTemplate.SlotCount;
	const FIntPoint& InventorySlotCount = InventoryManager->GetInventorySlotCount();
	TArray<TArray<bool>> TempSlotChecks = InventoryManager->GetSlotChecks();
	
	const FIntPoint StartSlotPos = FIntPoint::ZeroValue;
	const FIntPoint EndSlotPos = InventorySlotCount - ItemSlotCount;
	
	for (int32 y = StartSlotPos.Y; y <= EndSlotPos.Y; y++)
	{
		for (int32 x = StartSlotPos.X; x <= EndSlotPos.X; x++)
		{
			if (TempSlotChecks[y][x])
				continue;

			FIntPoint ItemSlotPos = FIntPoint(x, y);
			if (InventoryManager->IsEmpty(TempSlotChecks, ItemSlotPos, ItemSlotCount))
			{
				InventoryManager->MarkSlotChecks(TempSlotChecks, true, ItemSlotPos, ItemSlotCount);
				
				int32 AddCount = ConsumableFragment ? FMath::Min(ItemCount, ConsumableFragment->MaxStackCount) : 1;
				NewQueue.Emplace(y * InventorySlotCount.X + x, AddCount);
				ItemCount -= AddCount;
				
				if (ItemCount == 0)
				{
					for (const auto& Pair : AddQueue)
					{
						FD1InventoryEntry& ToEntry = Entries[Pair.Key];
						ToEntry.ItemCount += Pair.Value;
						MarkItemDirty(ToEntry);
					}
					
					for (const auto& Pair : NewQueue)
					{
						FIntPoint ToSlotPos = FIntPoint(Pair.Key % InventorySlotCount.X, Pair.Key / InventorySlotCount.X);
						InventoryManager->MarkSlotChecks(true, ToSlotPos, ItemSlotCount);
						
						FD1InventoryEntry& Entry = Entries[Pair.Key];
						AddedItemInstances.Add(Entry.Init(ItemTemplateID, Pair.Value, ItemRarity));
						
						MarkItemDirty(Entry);
					}
					return AddedItemInstances;
				}
			}
		}
	}
	return AddedItemInstances;
}

TArray<UD1ItemInstance*> FD1InventoryList::TryRemoveItem(int32 ItemTemplateID, int32 ItemCount)
{
	TArray<UD1ItemInstance*> RemovedItemInstances;
	
	if (ItemTemplateID <= 0 || ItemCount <= 0)
		return RemovedItemInstances;

	if (GetTotalCountByID(ItemTemplateID) < ItemCount)
		return RemovedItemInstances;
	
	const UD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(ItemTemplateID);

	TArray<TPair<int32/*EntryIndex*/, int32/*RemoveCount*/>> RemoveQueue;
	
	for (int32 i = Entries.Num() - 1; i >= 0; i--)
	{
		const FD1InventoryEntry& Entry = Entries[i];
		if (Entry.ItemInstance == nullptr)
			continue;

		if (Entry.ItemInstance->GetItemTemplateID() != ItemTemplateID)
			continue;
			
		if (Entry.ItemCount < ItemCount)
		{
			RemoveQueue.Emplace(i, Entry.ItemCount);
			ItemCount -= Entry.ItemCount;
		}
		else
		{
			RemoveQueue.Emplace(i, ItemCount);
			
			const FIntPoint& InventorySlotCount = InventoryManager->GetInventorySlotCount();
			for(const auto& Pair : RemoveQueue)
			{
				FD1InventoryEntry& ToEntry = Entries[Pair.Key];
				ToEntry.ItemCount -= Pair.Value;

				if (ToEntry.ItemCount == 0)
				{
					FIntPoint ItemSlotPos = FIntPoint(Pair.Key % InventorySlotCount.X, Pair.Key / InventorySlotCount.X);
					InventoryManager->MarkSlotChecks(false, ItemSlotPos, ItemTemplate.SlotCount);
					RemovedItemInstances.Add(ToEntry.Reset());
				}
				MarkItemDirty(ToEntry);
			}
			return RemovedItemInstances;
		}
	}
	return RemovedItemInstances;
}

void FD1InventoryList::BroadcastChangedMessage(const FIntPoint& ItemSlotPos, UD1ItemInstance* ItemInstance, int32 ItemCount)
{
	FD1InventoryEntryChangedMessage Message;
	Message.ItemSlotPos = ItemSlotPos;
	Message.ItemInstance = ItemInstance;
	Message.ItemCount = ItemCount;

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(InventoryManager->GetWorld());
	MessageSubsystem.BroadcastMessage(D1GameplayTags::Message_Item_InventoryEntryChanged, Message);
}

int32 FD1InventoryList::GetTotalCountByID(int32 ItemTemplateID) const
{
	int32 Count = 0;
	for (const FD1InventoryEntry& Entry : Entries)
	{
		if (Entry.ItemInstance && Entry.ItemInstance->GetItemTemplateID() == ItemTemplateID)
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
	}
	
	SlotChecks.SetNumZeroed(InventorySlotCount.Y);
	for (int32 y = 0; y < SlotChecks.Num(); y++)
	{
		SlotChecks[y].SetNumZeroed(InventorySlotCount.X);
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

void UD1InventoryManagerComponent::Server_RequestMoveOrMergeItem_FromInternalInventory_Implementation(const FIntPoint& FromItemSlotPos, const FIntPoint& ToItemSlotPos)
{
	check(GetOwner()->HasAuthority());
	
	int32 MovableItemCount = CanMoveOrMergeItem_FromInternalInventory(FromItemSlotPos, ToItemSlotPos);
	if (MovableItemCount > 0)
	{
		TArray<FD1InventoryEntry>& Entries = InventoryList.Entries;
		const int32 FromEntryIndex = FromItemSlotPos.Y * InventorySlotCount.X + FromItemSlotPos.X;
		FD1InventoryEntry& FromEntry = Entries[FromEntryIndex];
		UD1ItemInstance* FromItemInstance = FromEntry.GetItemInstance();

		FromEntry.ItemCount -= MovableItemCount;
		if (FromEntry.GetItemCount() <= 0)
		{
			UD1ItemInstance* RemovedItemInstance = FromEntry.Reset();
			if (IsUsingRegisteredSubObjectList())
			{
				if (RemovedItemInstance)
				{
					RemoveReplicatedSubObject(RemovedItemInstance);
				}
			}
		}
		
		const int32 ToEntryIndex = ToItemSlotPos.Y * InventorySlotCount.X + ToItemSlotPos.X;
		FD1InventoryEntry& ToEntry = Entries[ToEntryIndex];

		UD1ItemInstance* ToItemInstance = NewObject<UD1ItemInstance>();
		ToItemInstance->ItemTemplateID = FromItemInstance->GetItemTemplateID();
		ToItemInstance->ItemRarity = FromItemInstance->GetItemRarity();
		ToItemInstance->StatContainer = FromItemInstance->GetStatContainer();

		ToEntry.Init(ToItemInstance, MovableItemCount);
		
		// UD1ItemInstance* RemovedItemInstance = InventoryList.RemoveItem_Unsafe(FromItemSlotPos, MovableItemCount);
		// InventoryList.AddItem_Unsafe(ToItemSlotPos, RemovedItemInstance, MovableItemCount);

		if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
		{
			if (ToItemInstance)
			{
				AddReplicatedSubObject(ToItemInstance);
			}
		}
	}
}

int32 UD1InventoryManagerComponent::CanMoveOrMergeItem_FromInternalInventory(const FIntPoint& FromItemSlotPos, const FIntPoint& ToItemSlotPos) const
{
	if (FromItemSlotPos.X < 0 || FromItemSlotPos.Y < 0 || FromItemSlotPos.X >= InventorySlotCount.X || FromItemSlotPos.Y >= InventorySlotCount.Y)
		return 0;

	if (ToItemSlotPos.X < 0 || ToItemSlotPos.Y < 0 || ToItemSlotPos.X >= InventorySlotCount.X || ToItemSlotPos.Y >= InventorySlotCount.Y)
		return 0;

	const TArray<FD1InventoryEntry>& Entries = GetAllEntries();

	const int32 FromIndex = FromItemSlotPos.Y * InventorySlotCount.X + FromItemSlotPos.X;
	const FD1InventoryEntry& FromEntry = Entries[FromIndex];
	const UD1ItemInstance* FromItemInstance = FromEntry.ItemInstance;
	const int32 FromItemCount = FromEntry.ItemCount;

	if (FromItemInstance == nullptr)
		return 0;

	if (FromItemSlotPos == ToItemSlotPos)
		return FromItemCount;
	
	const int32 ToIndex = ToItemSlotPos.Y * InventorySlotCount.X + ToItemSlotPos.X;
	const FD1InventoryEntry& ToEntry = Entries[ToIndex];
	const UD1ItemInstance* ToItemInstance = ToEntry.ItemInstance;
	const int32 ToItemCount = ToEntry.ItemCount;
	
	const int32 FromTemplateID = FromItemInstance->GetItemTemplateID();
	const UD1ItemTemplate& FromItemTemplate = UD1ItemData::Get().FindItemTemplateByID(FromTemplateID);
	
	if (ToItemInstance)
	{
		const int32 ToTemplateID = ToItemInstance->GetItemTemplateID();
		if (FromTemplateID != ToTemplateID)
			return 0;

		const UD1ItemFragment_Consumable* ConsumableFragment = FromItemTemplate.FindFragmentByClass<UD1ItemFragment_Consumable>();
		if (ConsumableFragment == nullptr)
			return 0;

		return FMath::Min(FromItemCount + ToItemCount, ConsumableFragment->MaxStackCount) - ToItemCount;
	}
	else
	{
		const FIntPoint& FromItemSlotCount = FromItemTemplate.SlotCount;
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
		return IsEmpty(ToItemSlotPos, FromItemSlotCount) ? FromItemCount : 0;
	}
}

void UD1InventoryManagerComponent::Server_RequestMoveOrMergeItem_FromExternalInventory_Implementation(UD1InventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, const FIntPoint& ToItemSlotPos)
{
	check(GetOwner()->HasAuthority());
	
	int32 MovableItemCount = CanMoveOrMergeItem_FromExternalInventory(OtherComponent, FromItemSlotPos, ToItemSlotPos);
	if (MovableItemCount > 0)
	{
		UD1ItemInstance* RemovedItemInstance = OtherComponent->InventoryList.RemoveItem_Unsafe(FromItemSlotPos, MovableItemCount);
		InventoryList.AddItem_Unsafe(ToItemSlotPos, RemovedItemInstance, MovableItemCount);
	}
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
	const int32 FromIndex = FromItemSlotPos.Y * FromInventorySlotCount.X + FromItemSlotPos.X;
	const FD1InventoryEntry& FromEntry = FromEntries[FromIndex];
	const UD1ItemInstance* FromItemInstance = FromEntry.ItemInstance;
	const int32 FromItemCount = FromEntry.ItemCount;

	if (FromItemInstance == nullptr)
		return 0;
	
	const TArray<FD1InventoryEntry>& ToEntries = GetAllEntries();
	const int32 ToIndex = ToItemSlotPos.Y * ToInventorySlotCount.X + ToItemSlotPos.X;
	const FD1InventoryEntry& ToEntry = ToEntries[ToIndex];
	const UD1ItemInstance* ToItemInstance = ToEntry.ItemInstance;
	const int32 ToItemCount = ToEntry.ItemCount;
	
	const int32 FromTemplateID = FromItemInstance->GetItemTemplateID();
	const UD1ItemTemplate& FromItemTemplate = UD1ItemData::Get().FindItemTemplateByID(FromTemplateID);
	
	if (ToItemInstance)
	{
		const int32 ToTemplateID = ToItemInstance->GetItemTemplateID();
		if (FromTemplateID != ToTemplateID)
			return 0;

		const UD1ItemFragment_Consumable* ConsumableFragment = FromItemTemplate.FindFragmentByClass<UD1ItemFragment_Consumable>();
		if (ConsumableFragment == nullptr)
			return 0;

		return FMath::Min(FromItemCount + ToItemCount, ConsumableFragment->MaxStackCount) - ToItemCount;
	}
	else
	{
		const FIntPoint& FromItemSlotCount = FromItemTemplate.SlotCount;
		if (ToItemSlotPos.X + FromItemSlotCount.X > InventorySlotCount.X || ToItemSlotPos.Y + FromItemSlotCount.Y > InventorySlotCount.Y)
			return 0;
		
		return IsEmpty(ToItemSlotPos, FromItemSlotCount) ? FromItemCount : 0;
	}
}

void UD1InventoryManagerComponent::Server_RequestMoveOrMergeItem_FromExternalEquipment_Implementation(UD1EquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, const FIntPoint& ToItemSlotPos)
{
	check(GetOwner()->HasAuthority());
	
	if (CanMoveOrMergeItem_FromExternalEquipment(OtherComponent, FromEquipmentSlotType, ToItemSlotPos))
	{
		UD1ItemInstance* RemovedItemInstance = OtherComponent->EquipmentList.RemoveEntry(FromEquipmentSlotType);
		InventoryList.AddItem_Unsafe(ToItemSlotPos, RemovedItemInstance, 1);
	}
}

bool UD1InventoryManagerComponent::CanMoveOrMergeItem_FromExternalEquipment(UD1EquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, const FIntPoint& ToItemSlotPos) const
{
	if (OtherComponent == nullptr)
		return false;

	if (FromEquipmentSlotType == EEquipmentSlotType::Unarmed_LeftHand || FromEquipmentSlotType == EEquipmentSlotType::Unarmed_RightHand || FromEquipmentSlotType == EEquipmentSlotType::Count)
		return false;
	
	if (ToItemSlotPos.X < 0 || ToItemSlotPos.Y < 0 || ToItemSlotPos.X >= InventorySlotCount.X || ToItemSlotPos.Y >= InventorySlotCount.Y)
		return false;

	const TArray<FD1EquipmentEntry>& FromEntries = OtherComponent->GetAllEntries();
	const FD1EquipmentEntry& FromEntry = FromEntries[(int32)FromEquipmentSlotType];
	const UD1ItemInstance* FromItemInstance = FromEntry.GetItemInstance();

	if (FromItemInstance == nullptr)
		return false;
	
	const TArray<FD1InventoryEntry>& ToEntries = GetAllEntries();
	const int32 ToIndex = ToItemSlotPos.Y * InventorySlotCount.X + ToItemSlotPos.X;
	const FD1InventoryEntry& ToEntry = ToEntries[ToIndex];
	const UD1ItemInstance* ToItemInstance = ToEntry.ItemInstance;

	if (ToItemInstance)
		return false;

	const int32 FromTemplateID = FromItemInstance->GetItemTemplateID();
	const UD1ItemTemplate& FromItemTemplate = UD1ItemData::Get().FindItemTemplateByID(FromTemplateID);

	const FIntPoint& FromItemSlotCount = FromItemTemplate.SlotCount;
	return IsEmpty(ToItemSlotPos, FromItemSlotCount);
}

void UD1InventoryManagerComponent::TryAddItem(int32 ItemTemplateID, int32 ItemCount, EItemRarity ItemRarity)
{
	check(GetOwner()->HasAuthority());
	
	TArray<UD1ItemInstance*> AddedItemInstances = InventoryList.TryAddItem(ItemTemplateID, ItemCount, ItemRarity);
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
}

void UD1InventoryManagerComponent::TryAddItem(int32 ItemTemplateID, int32 ItemCount, const TArray<FD1ItemRarityProbability>& ItemProbabilities)
{
	check(GetOwner()->HasAuthority());

	TryAddItem(ItemTemplateID, ItemCount, UD1ItemInstance::DetermineItemRarity(ItemProbabilities));
}

void UD1InventoryManagerComponent::TryRemoveItem(int32 ItemTemplateID, int32 ItemCount)
{
	check(GetOwner()->HasAuthority());
	
	TArray<UD1ItemInstance*> RemovedItemInstances = InventoryList.TryRemoveItem(ItemTemplateID, ItemCount);
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
}

const TArray<FD1InventoryEntry>& UD1InventoryManagerComponent::GetAllEntries() const
{
	return InventoryList.GetAllEntries();
}

int32 UD1InventoryManagerComponent::GetTotalCountByID(int32 ItemTemplateID) const
{
	return InventoryList.GetTotalCountByID(ItemTemplateID);
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
