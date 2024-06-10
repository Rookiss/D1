#include "D1EquipmentManagerComponent.h"

#include "D1EquipManagerComponent.h"
#include "D1InventoryManagerComponent.h"
#include "D1GameplayTags.h"
#include "Character/LyraCharacter.h"
#include "Data/D1ItemData.h"
#include "Engine/ActorChannel.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Item/D1ItemInstance.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Armor.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Weapon.h"
#include "Net/UnrealNetwork.h"
#include "Player/LyraPlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1EquipmentManagerComponent)

void FD1EquipmentEntry::Init(UD1ItemInstance* NewItemInstance)
{
	if (ItemInstance == NewItemInstance)
		return;

	ALyraCharacter* Character = EquipmentManager->GetCharacter();
	check(Character);
	
	UD1EquipManagerComponent* EquipManager = Character->FindComponentByClass<UD1EquipManagerComponent>();
	check(EquipManager);
	
	if (ItemInstance)
	{
		EquipManager->Unequip(EquipmentSlotType);
	}
	
	ItemInstance = NewItemInstance;
	
	if (ItemInstance)
	{
		LastValidTemplateID = ItemInstance->GetItemTemplateID();

		const UD1ItemFragment_Equippable_Armor* Armor = ItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable_Armor>();
		const UD1ItemFragment_Equippable_Weapon* Weapon = ItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable_Weapon>();
		if (Armor || Weapon && EquipmentManager->IsSameWeaponEquipState(EquipmentSlotType, EquipManager->GetCurrentWeaponEquipState()))
		{
			EquipManager->Equip(EquipmentSlotType, ItemInstance);
		}
	}
	else
	{
		if (EquipmentManager->IsAllEmpty(EquipManager->GetCurrentWeaponEquipState()))
		{
			EquipManager->ChangeWeaponEquipState(EWeaponEquipState::Unarmed);
		}
	}
}

bool FD1EquipmentList::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
{
	return FFastArraySerializer::FastArrayDeltaSerialize<FD1EquipmentEntry, FD1EquipmentList>(Entries, DeltaParams,*this);
}

void FD1EquipmentList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		FD1EquipmentEntry& Entry = Entries[Index];
		Entry.EquipmentManager = EquipmentManager;
		Entry.EquipmentSlotType = (EEquipmentSlotType)Index;
		
		if (UD1ItemInstance* ItemInstance = Entry.ItemInstance)
		{
			BroadcastChangedMessage((EEquipmentSlotType)Index, ItemInstance);
		}
	}
}

void FD1EquipmentList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		const FD1EquipmentEntry& Entry = Entries[Index];
		BroadcastChangedMessage((EEquipmentSlotType)Index, Entry.ItemInstance);
	}
}

void FD1EquipmentList::AddEquipment(EEquipmentSlotType EquipmentSlotType, UD1ItemInstance* ItemInstance)
{
	FD1EquipmentEntry& Entry = Entries[(int32)EquipmentSlotType];
	Entry.Init(ItemInstance);
	MarkItemDirty(Entry);
}

UD1ItemInstance* FD1EquipmentList::RemoveEquipment(EEquipmentSlotType EquipmentSlotType)
{
	FD1EquipmentEntry& Entry = Entries[(int32)EquipmentSlotType];
	UD1ItemInstance* ItemInstance = Entry.ItemInstance;
	Entry.Init(nullptr);
	MarkItemDirty(Entry);
	return ItemInstance;
}

void FD1EquipmentList::BroadcastChangedMessage(EEquipmentSlotType EquipmentSlotType, UD1ItemInstance* ItemInstance)
{
	if (EquipmentManager->OnEquipmentEntryChanged.IsBound())
	{
		EquipmentManager->OnEquipmentEntryChanged.Broadcast(EquipmentSlotType, ItemInstance);
	}
}

UD1EquipmentManagerComponent::UD1EquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, EquipmentList(this)
{
	bWantsInitializeComponent = true;
    SetIsReplicatedByDefault(true);
}

void UD1EquipmentManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();
	
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		TArray<FD1EquipmentEntry>& Entries = EquipmentList.Entries;
		Entries.SetNum((int32)EEquipmentSlotType::Count);

		for (int32 i = 0; i < Entries.Num(); i++)
		{
			FD1EquipmentEntry& Entry = Entries[i];
			Entry.EquipmentManager = this;
			Entry.EquipmentSlotType = (EEquipmentSlotType)i;
			EquipmentList.MarkItemDirty(Entry);
		}
	}
}

void UD1EquipmentManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EquipmentList);
}

bool UD1EquipmentManagerComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	
	for (FD1EquipmentEntry& Entry : EquipmentList.Entries)
	{
		UD1ItemInstance* ItemInstance = Entry.ItemInstance;
		if (IsValid(ItemInstance))
		{
			bWroteSomething |= Channel->ReplicateSubobject(ItemInstance, *Bunch, *RepFlags);
		}
	}
	
	return bWroteSomething;
}

void UD1EquipmentManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	if (IsUsingRegisteredSubObjectList())
	{
		for (const FD1EquipmentEntry& Entry : EquipmentList.Entries)
		{
			UD1ItemInstance* ItemInstance = Entry.ItemInstance;
			if (IsValid(ItemInstance))
			{
				AddReplicatedSubObject(ItemInstance);
			}
		}
	}
}

void UD1EquipmentManagerComponent::Server_AddEquipment_FromInventory_Implementation(UD1InventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, EEquipmentSlotType ToEquipmentSlotType)
{
	check(GetOwner()->HasAuthority());
	
	if (CanAddEquipment_FromInventory(OtherComponent, FromItemSlotPos, ToEquipmentSlotType))
	{
		UD1ItemInstance* RemovedItemInstance = OtherComponent->RemoveItem_Unsafe(FromItemSlotPos, 1);
		EquipmentList.AddEquipment(ToEquipmentSlotType, RemovedItemInstance);
	}
}

bool UD1EquipmentManagerComponent::CanAddEquipment_FromInventory(UD1InventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, EEquipmentSlotType ToEquipmentSlotType) const
{
	if (OtherComponent == nullptr)
		return false;

	const FIntPoint& FromTotalItemSlotCount = OtherComponent->GetInventorySlotCount();
	if (FromItemSlotPos.X < 0 || FromItemSlotPos.Y < 0 || FromItemSlotPos.X >= FromTotalItemSlotCount.X || FromItemSlotPos.Y >= FromTotalItemSlotCount.Y)
		return false;
	
	const TArray<FD1InventoryEntry>& FromEntries = OtherComponent->GetAllEntries();
	const int32 FromIndex = FromItemSlotPos.Y * FromTotalItemSlotCount.X + FromItemSlotPos.X;
	const FD1InventoryEntry& FromEntry = FromEntries[FromIndex];
	UD1ItemInstance* FromItemInstance = FromEntry.GetItemInstance();

	return CanAddEquipment(FromItemInstance, ToEquipmentSlotType);
}

void UD1EquipmentManagerComponent::Server_AddEquipment_FromEquipment_Implementation(UD1EquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, EEquipmentSlotType ToEquipmentSlotType)
{
	check(GetOwner()->HasAuthority());

	if (this == OtherComponent && FromEquipmentSlotType == ToEquipmentSlotType)
		return;
	
	if (CanAddEquipment_FromEquipment(OtherComponent, FromEquipmentSlotType, ToEquipmentSlotType))
	{
		UD1ItemInstance* RemovedItemInstance = OtherComponent->EquipmentList.RemoveEquipment(FromEquipmentSlotType);
		EquipmentList.AddEquipment(ToEquipmentSlotType, RemovedItemInstance);
	}
}

bool UD1EquipmentManagerComponent::CanAddEquipment_FromEquipment(UD1EquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, EEquipmentSlotType ToEquipmentSlotType) const
{
	if (OtherComponent == nullptr)
		return false;

	if (FromEquipmentSlotType == EEquipmentSlotType::Unarmed_LeftHand || FromEquipmentSlotType == EEquipmentSlotType::Unarmed_RightHand)
		return false;

	if (ToEquipmentSlotType == EEquipmentSlotType::Unarmed_LeftHand || ToEquipmentSlotType == EEquipmentSlotType::Unarmed_RightHand)
		return false;
	
	if (FromEquipmentSlotType == EEquipmentSlotType::Count || ToEquipmentSlotType == EEquipmentSlotType::Count)
		return false;
	
	const TArray<FD1EquipmentEntry>& FromEntries = OtherComponent->GetAllEntries();
	const FD1EquipmentEntry& FromEntry = FromEntries[(int32)FromEquipmentSlotType];
	UD1ItemInstance* FromItemInstance = FromEntry.GetItemInstance();

	return CanAddEquipment(FromItemInstance, ToEquipmentSlotType);
}

bool UD1EquipmentManagerComponent::CanAddEquipment(UD1ItemInstance* FromItemInstance, EEquipmentSlotType ToEquipmentSlotType) const
{
	if (FromItemInstance == nullptr)
		return false;
	
	if (ToEquipmentSlotType == EEquipmentSlotType::Unarmed_LeftHand || ToEquipmentSlotType == EEquipmentSlotType::Unarmed_RightHand || ToEquipmentSlotType == EEquipmentSlotType::Count)
		return false;

	const TArray<FD1EquipmentEntry>& Entries = GetAllEntries();
	const FD1EquipmentEntry& ToEntry = Entries[(int32)ToEquipmentSlotType];
	if (FromItemInstance == ToEntry.ItemInstance)
		return true;

	if (ToEntry.ItemInstance)
		return false;
	
	const UD1ItemFragment_Equippable* FromEquippableFragment = FromItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable>();
	if (FromEquippableFragment == nullptr)
		return false;
	
	if (FromEquippableFragment->EquipmentType == EEquipmentType::Weapon)
	{
		const UD1ItemFragment_Equippable_Weapon* FromWeaponFragment = Cast<UD1ItemFragment_Equippable_Weapon>(FromEquippableFragment);
		EWeaponHandType FromWeaponHandType = FromWeaponFragment->WeaponHandType;

		if (IsSameWeaponHandType(ToEquipmentSlotType, FromWeaponHandType) == false)
			return false;
		
		if (IsPrimaryWeaponSlot(ToEquipmentSlotType))
		{
			if (FromWeaponHandType == EWeaponHandType::LeftHand || FromWeaponHandType == EWeaponHandType::RightHand)
			{
				const FD1EquipmentEntry& TwoHandEntry = Entries[(int32)EEquipmentSlotType::Primary_TwoHand];
				return (TwoHandEntry.ItemInstance == nullptr);
			}
			else if (FromWeaponHandType == EWeaponHandType::TwoHand)
			{
				const FD1EquipmentEntry& LeftHandEntry = Entries[(int32)EEquipmentSlotType::Primary_LeftHand];
				const FD1EquipmentEntry& RightHandEntry = Entries[(int32)EEquipmentSlotType::Primary_RightHand];
				return (LeftHandEntry.ItemInstance == nullptr && RightHandEntry.ItemInstance == nullptr);
			}
		}
		else if (IsSecondaryWeaponSlot(ToEquipmentSlotType))
		{
			if (FromWeaponHandType == EWeaponHandType::LeftHand || FromWeaponHandType == EWeaponHandType::RightHand)
			{
				const FD1EquipmentEntry& TwoHandEntry = Entries[(int32)EEquipmentSlotType::Secondary_TwoHand];
				return (TwoHandEntry.ItemInstance == nullptr);
			}
			else if (FromWeaponHandType == EWeaponHandType::TwoHand)
			{
				const FD1EquipmentEntry& LeftHandEntry = Entries[(int32)EEquipmentSlotType::Secondary_LeftHand];
				const FD1EquipmentEntry& RightHandEntry = Entries[(int32)EEquipmentSlotType::Secondary_RightHand];
				return (LeftHandEntry.ItemInstance == nullptr && RightHandEntry.ItemInstance == nullptr);
			}
		}
		else if (IsTertiaryWeaponSlot(ToEquipmentSlotType))
		{
			if (FromWeaponHandType == EWeaponHandType::LeftHand || FromWeaponHandType == EWeaponHandType::RightHand)
			{
				const FD1EquipmentEntry& TwoHandEntry = Entries[(int32)EEquipmentSlotType::Tertiary_TwoHand];
				return (TwoHandEntry.ItemInstance == nullptr);
			}
			else if (FromWeaponHandType == EWeaponHandType::TwoHand)
			{
				const FD1EquipmentEntry& LeftHandEntry = Entries[(int32)EEquipmentSlotType::Tertiary_LeftHand];
				const FD1EquipmentEntry& RightHandEntry = Entries[(int32)EEquipmentSlotType::Tertiary_RightHand];
				return (LeftHandEntry.ItemInstance == nullptr && RightHandEntry.ItemInstance == nullptr);
			}
		}
		else if (IsQuaternaryWeaponSlot(ToEquipmentSlotType))
		{
			if (FromWeaponHandType == EWeaponHandType::LeftHand || FromWeaponHandType == EWeaponHandType::RightHand)
			{
				const FD1EquipmentEntry& TwoHandEntry = Entries[(int32)EEquipmentSlotType::Quaternary_TwoHand];
				return (TwoHandEntry.ItemInstance == nullptr);
			}
			else if (FromWeaponHandType == EWeaponHandType::TwoHand)
			{
				const FD1EquipmentEntry& LeftHandEntry = Entries[(int32)EEquipmentSlotType::Quaternary_LeftHand];
				const FD1EquipmentEntry& RightHandEntry = Entries[(int32)EEquipmentSlotType::Quaternary_RightHand];
				return (LeftHandEntry.ItemInstance == nullptr && RightHandEntry.ItemInstance == nullptr);
			}
		}
	}
	else if (FromEquippableFragment->EquipmentType == EEquipmentType::Armor)
	{
		const UD1ItemFragment_Equippable_Armor* FromArmorFragment = Cast<UD1ItemFragment_Equippable_Armor>(FromEquippableFragment);
		return IsSameArmorType(ToEquipmentSlotType, FromArmorFragment->ArmorType);
	}
	return false;
}

void UD1EquipmentManagerComponent::TryAddEquipment(EEquipmentSlotType EquipmentSlotType, TSubclassOf<UD1ItemTemplate> ItemTemplateClass, EItemRarity ItemRarity)
{
	check(HasAuthority());

	if (EquipmentSlotType == EEquipmentSlotType::Count || ItemTemplateClass == nullptr || ItemRarity == EItemRarity::Count)
		return;

	TArray<FD1EquipmentEntry>& Entries = EquipmentList.Entries;
	FD1EquipmentEntry& Entry = Entries[(int32)EquipmentSlotType];

	if (Entry.ItemInstance == nullptr)
	{
		UD1ItemInstance* AddedItemInstance = NewObject<UD1ItemInstance>();
		int32 ItemTemplateID = UD1ItemData::Get().FindItemTemplateIDByClass(ItemTemplateClass);
		AddedItemInstance->Init(ItemTemplateID, ItemRarity);
		EquipmentList.AddEquipment(EquipmentSlotType, AddedItemInstance);

		if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && AddedItemInstance)
		{
			AddReplicatedSubObject(AddedItemInstance);
		}
	}
}

void UD1EquipmentManagerComponent::AddEquipment_Unsafe(EEquipmentSlotType EquipmentSlotType, UD1ItemInstance* ItemInstance)
{
	check(HasAuthority());

	if (EquipmentSlotType == EEquipmentSlotType::Count || ItemInstance == nullptr)
		return;

	TArray<FD1EquipmentEntry>& Entries = EquipmentList.Entries;
	FD1EquipmentEntry& Entry = Entries[(int32)EquipmentSlotType];
	
	Entry.Init(ItemInstance);
	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && ItemInstance)
	{
		AddReplicatedSubObject(ItemInstance);
	}
}

UD1ItemInstance* UD1EquipmentManagerComponent::RemoveEquipment_Unsafe(EEquipmentSlotType EquipmentSlotType)
{
	check(HasAuthority());

	UD1ItemInstance* RemovedItemInstance = nullptr;
	
	if (EquipmentSlotType == EEquipmentSlotType::Count)
		return RemovedItemInstance;

	TArray<FD1EquipmentEntry>& Entries = EquipmentList.Entries;
	FD1EquipmentEntry& Entry = Entries[(int32)EquipmentSlotType];

	if (Entry.ItemInstance)
	{
		RemovedItemInstance = EquipmentList.RemoveEquipment(EquipmentSlotType);
		if (IsUsingRegisteredSubObjectList() && RemovedItemInstance)
		{
			RemoveReplicatedSubObject(RemovedItemInstance);
		}
	}
	return RemovedItemInstance;
}

bool UD1EquipmentManagerComponent::IsWeaponSlot(EEquipmentSlotType EquipmentSlotType)
{
	return (EquipmentSlotType == EEquipmentSlotType::Unarmed_LeftHand	 || EquipmentSlotType == EEquipmentSlotType::Unarmed_RightHand	  ||
			EquipmentSlotType == EEquipmentSlotType::Primary_LeftHand	 || EquipmentSlotType == EEquipmentSlotType::Primary_RightHand	  || EquipmentSlotType == EEquipmentSlotType::Primary_TwoHand   ||
			EquipmentSlotType == EEquipmentSlotType::Secondary_LeftHand  || EquipmentSlotType == EEquipmentSlotType::Secondary_RightHand  || EquipmentSlotType == EEquipmentSlotType::Secondary_TwoHand ||
			EquipmentSlotType == EEquipmentSlotType::Tertiary_LeftHand	 || EquipmentSlotType == EEquipmentSlotType::Tertiary_RightHand   || EquipmentSlotType == EEquipmentSlotType::Tertiary_TwoHand  ||
			EquipmentSlotType == EEquipmentSlotType::Quaternary_LeftHand || EquipmentSlotType == EEquipmentSlotType::Quaternary_RightHand || EquipmentSlotType == EEquipmentSlotType::Quaternary_TwoHand);
}

bool UD1EquipmentManagerComponent::IsArmorSlot(EEquipmentSlotType EquipmentSlotType)
{
	return (EquipmentSlotType == EEquipmentSlotType::Helmet || EquipmentSlotType == EEquipmentSlotType::Chest || EquipmentSlotType == EEquipmentSlotType::Legs   || EquipmentSlotType == EEquipmentSlotType::Hands || EquipmentSlotType == EEquipmentSlotType::Foot);
}

bool UD1EquipmentManagerComponent::IsSameWeaponEquipState(EEquipmentSlotType EquipmentSlotType, EWeaponEquipState WeaponEquipState)
{
	return ((EquipmentSlotType == EEquipmentSlotType::Unarmed_LeftHand    || EquipmentSlotType == EEquipmentSlotType::Unarmed_RightHand) && WeaponEquipState == EWeaponEquipState::Unarmed   ||
			(EquipmentSlotType == EEquipmentSlotType::Primary_LeftHand    || EquipmentSlotType == EEquipmentSlotType::Primary_RightHand    || EquipmentSlotType == EEquipmentSlotType::Primary_TwoHand)    && WeaponEquipState == EWeaponEquipState::Primary   ||
		    (EquipmentSlotType == EEquipmentSlotType::Secondary_LeftHand  || EquipmentSlotType == EEquipmentSlotType::Secondary_RightHand  || EquipmentSlotType == EEquipmentSlotType::Secondary_TwoHand)  && WeaponEquipState == EWeaponEquipState::Secondary ||
		    (EquipmentSlotType == EEquipmentSlotType::Tertiary_LeftHand	  || EquipmentSlotType == EEquipmentSlotType::Tertiary_RightHand   || EquipmentSlotType == EEquipmentSlotType::Tertiary_TwoHand)   && WeaponEquipState == EWeaponEquipState::Tertiary  ||
		    (EquipmentSlotType == EEquipmentSlotType::Quaternary_LeftHand || EquipmentSlotType == EEquipmentSlotType::Quaternary_RightHand || EquipmentSlotType == EEquipmentSlotType::Quaternary_TwoHand) && WeaponEquipState == EWeaponEquipState::Quaternary);
}

bool UD1EquipmentManagerComponent::IsSameWeaponHandType(EEquipmentSlotType EquipmentSlotType, EWeaponHandType WeaponHandType)
{
	return ((EquipmentSlotType == EEquipmentSlotType::Primary_LeftHand  || EquipmentSlotType == EEquipmentSlotType::Secondary_LeftHand  || EquipmentSlotType == EEquipmentSlotType::Tertiary_LeftHand  || EquipmentSlotType == EEquipmentSlotType::Quaternary_LeftHand)  && WeaponHandType == EWeaponHandType::LeftHand  ||
			(EquipmentSlotType == EEquipmentSlotType::Primary_RightHand || EquipmentSlotType == EEquipmentSlotType::Secondary_RightHand || EquipmentSlotType == EEquipmentSlotType::Tertiary_RightHand || EquipmentSlotType == EEquipmentSlotType::Quaternary_RightHand) && WeaponHandType == EWeaponHandType::RightHand ||
			(EquipmentSlotType == EEquipmentSlotType::Primary_TwoHand   || EquipmentSlotType == EEquipmentSlotType::Secondary_TwoHand   || EquipmentSlotType == EEquipmentSlotType::Tertiary_TwoHand   || EquipmentSlotType == EEquipmentSlotType::Quaternary_TwoHand)   && WeaponHandType == EWeaponHandType::TwoHand);
}

bool UD1EquipmentManagerComponent::IsSameArmorType(EEquipmentSlotType EquipmentSlotType, EArmorType ArmorType)
{
	return (EquipmentSlotType == EEquipmentSlotType::Helmet && ArmorType == EArmorType::Helmet || EquipmentSlotType == EEquipmentSlotType::Chest && ArmorType == EArmorType::Chest ||
			EquipmentSlotType == EEquipmentSlotType::Legs   && ArmorType == EArmorType::Legs   || EquipmentSlotType == EEquipmentSlotType::Hands && ArmorType == EArmorType::Hands ||
			EquipmentSlotType == EEquipmentSlotType::Foot   && ArmorType == EArmorType::Foot);
}

bool UD1EquipmentManagerComponent::IsPrimaryWeaponSlot(EEquipmentSlotType EquipmentSlotType)
{
	return (EquipmentSlotType == EEquipmentSlotType::Primary_LeftHand || EquipmentSlotType == EEquipmentSlotType::Primary_RightHand || EquipmentSlotType == EEquipmentSlotType::Primary_TwoHand);
}

bool UD1EquipmentManagerComponent::IsSecondaryWeaponSlot(EEquipmentSlotType EquipmentSlotType)
{
	return (EquipmentSlotType == EEquipmentSlotType::Secondary_LeftHand || EquipmentSlotType == EEquipmentSlotType::Secondary_RightHand || EquipmentSlotType == EEquipmentSlotType::Secondary_TwoHand);
}

bool UD1EquipmentManagerComponent::IsTertiaryWeaponSlot(EEquipmentSlotType EquipmentSlotType)
{
	return (EquipmentSlotType == EEquipmentSlotType::Tertiary_LeftHand || EquipmentSlotType == EEquipmentSlotType::Tertiary_RightHand || EquipmentSlotType == EEquipmentSlotType::Tertiary_TwoHand);
}

bool UD1EquipmentManagerComponent::IsQuaternaryWeaponSlot(EEquipmentSlotType EquipmentSlotType)
{
	return (EquipmentSlotType == EEquipmentSlotType::Quaternary_LeftHand || EquipmentSlotType == EEquipmentSlotType::Quaternary_RightHand || EquipmentSlotType == EEquipmentSlotType::Quaternary_TwoHand);
}

bool UD1EquipmentManagerComponent::IsAllEmpty(EWeaponEquipState WeaponEquipState) const
{
	if (WeaponEquipState == EWeaponEquipState::Count)
		return true;

	if (WeaponEquipState == EWeaponEquipState::Unarmed)
		return false;

	bool bAllEmpty = true;
	for (EEquipmentSlotType SlotType : Item::EquipmentSlotsByWeaponState[(int32)WeaponEquipState])
	{
		const FD1EquipmentEntry& Entry = EquipmentList.Entries[(int32)SlotType];
		if (Entry.ItemInstance)
		{
			bAllEmpty = false;
			break;
		}
	}
	return bAllEmpty;
}

ALyraCharacter* UD1EquipmentManagerComponent::GetCharacter() const
{
	if (ALyraPlayerController* PlayerController = GetPlayerController())
	{
		return Cast<ALyraCharacter>(PlayerController->GetPawn());
	}
	return nullptr;
}

ALyraPlayerController* UD1EquipmentManagerComponent::GetPlayerController() const
{
	return Cast<ALyraPlayerController>(GetOwner());
}

const TArray<FD1EquipmentEntry>& UD1EquipmentManagerComponent::GetAllEntries() const
{
	return EquipmentList.GetAllEntries();
}
