#include "D1EquipmentManagerComponent.h"

#include "AbilitySystemGlobals.h"
#include "D1EquipManagerComponent.h"
#include "D1InventoryManagerComponent.h"
#include "AbilitySystem/D1AbilitySystemComponent.h"
#include "Character/D1Player.h"
#include "Engine/ActorChannel.h"
#include "Item/D1ItemInstance.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Armor.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Weapon.h"
#include "Net/UnrealNetwork.h"
#include "Player/D1PlayerController.h"
#include "Actors/D1WeaponBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1EquipmentManagerComponent)

void FD1EquipmentEntry::Init(UD1ItemInstance* NewItemInstance)
{
	if (ItemInstance == NewItemInstance)
		return;

	AD1Player* Player = EquipmentManager->GetPlayerCharacter();
	UD1EquipManagerComponent* EquipManager = Player->EquipManagerComponent;
	
	if (ItemInstance)
	{
		EquipManager->Unequip(EquipmentSlotType);
	}
	
	ItemInstance = NewItemInstance;
	if (ItemInstance)
	{
		LastValidTemplateID = ItemInstance->GetTemplateID();

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
			if (EquipmentManager->OnEquipmentEntryChanged.IsBound())
			{
				EquipmentManager->OnEquipmentEntryChanged.Broadcast((EEquipmentSlotType)Index, ItemInstance);
			}
		}
	}
}

void FD1EquipmentList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		const FD1EquipmentEntry& Entry = Entries[Index];
		if (EquipmentManager->OnEquipmentEntryChanged.IsBound())
		{
			EquipmentManager->OnEquipmentEntryChanged.Broadcast((EEquipmentSlotType)Index, Entry.ItemInstance);
		}
	}
}

void FD1EquipmentList::AddEntry(EEquipmentSlotType EquipmentSlotType, UD1ItemInstance* ItemInstance)
{
	FD1EquipmentEntry& Entry = Entries[(int32)EquipmentSlotType];
	Entry.Init(ItemInstance);
	MarkItemDirty(Entry);
}

UD1ItemInstance* FD1EquipmentList::RemoveEntry(EEquipmentSlotType EquipmentSlotType)
{
	FD1EquipmentEntry& Entry = Entries[(int32)EquipmentSlotType];
	UD1ItemInstance* ItemInstance = Entry.ItemInstance;
	Entry.Init(nullptr);
	MarkItemDirty(Entry);
	return ItemInstance;
}

UD1EquipmentManagerComponent::UD1EquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, EquipmentList(this)
{
    SetIsReplicatedByDefault(true);
}

void UD1EquipmentManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	
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

void UD1EquipmentManagerComponent::AddUnarmedEquipment()
{
	FD1EquipmentEntry& EntryLH = EquipmentList.Entries[(int32)EEquipmentSlotType::Unarmed_LeftHand];
	UD1ItemInstance* ItemInstanceLH = NewObject<UD1ItemInstance>();
	ItemInstanceLH->Init(Item::UnarmedLeftID);
	EntryLH.Init(ItemInstanceLH);

	FD1EquipmentEntry& EntryRH = EquipmentList.Entries[(int32)EEquipmentSlotType::Unarmed_RightHand];
	UD1ItemInstance* ItemInstanceRH = NewObject<UD1ItemInstance>();
	ItemInstanceRH->Init(Item::UnarmedRightID);
	EntryRH.Init(ItemInstanceRH);
}

void UD1EquipmentManagerComponent::Server_AddEntry_FromInventory_Implementation(UD1InventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, EEquipmentSlotType ToEquipmentSlotType)
{
	check(GetOwner()->HasAuthority());
	
	if (CanAddEntry_FromInventory(OtherComponent, FromItemSlotPos, ToEquipmentSlotType))
	{
		UD1ItemInstance* RemovedItemInstance = OtherComponent->InventoryList.RemoveItem_Unsafe(FromItemSlotPos, 1);
		EquipmentList.AddEntry(ToEquipmentSlotType, RemovedItemInstance);
	}
}

bool UD1EquipmentManagerComponent::CanAddEntry_FromInventory(UD1InventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, EEquipmentSlotType ToEquipmentSlotType) const
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

	return CanAddEntry(FromItemInstance, ToEquipmentSlotType);
}

void UD1EquipmentManagerComponent::Server_AddEntry_FromEquipment_Implementation(UD1EquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, EEquipmentSlotType ToEquipmentSlotType)
{
	check(GetOwner()->HasAuthority());

	if (this == OtherComponent && FromEquipmentSlotType == ToEquipmentSlotType)
		return;
	
	if (CanAddEntry_FromEquipment(OtherComponent, FromEquipmentSlotType, ToEquipmentSlotType))
	{
		UD1ItemInstance* RemovedItemInstance = OtherComponent->EquipmentList.RemoveEntry(FromEquipmentSlotType);
		EquipmentList.AddEntry(ToEquipmentSlotType, RemovedItemInstance);
	}
}

bool UD1EquipmentManagerComponent::CanAddEntry_FromEquipment(UD1EquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, EEquipmentSlotType ToEquipmentSlotType) const
{
	if (OtherComponent == nullptr)
		return false;

	if (FromEquipmentSlotType == EEquipmentSlotType::Unarmed_LeftHand || ToEquipmentSlotType == EEquipmentSlotType::Unarmed_RightHand)
		return false;
	
	if (FromEquipmentSlotType == EEquipmentSlotType::Count || ToEquipmentSlotType == EEquipmentSlotType::Count)
		return false;
	
	const TArray<FD1EquipmentEntry>& FromEntries = OtherComponent->GetAllEntries();
	const FD1EquipmentEntry& FromEntry = FromEntries[(int32)FromEquipmentSlotType];
	UD1ItemInstance* FromItemInstance = FromEntry.GetItemInstance();

	return CanAddEntry(FromItemInstance, ToEquipmentSlotType);
}

bool UD1EquipmentManagerComponent::CanAddEntry(UD1ItemInstance* FromItemInstance, EEquipmentSlotType ToEquipmentSlotType) const
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
	
	const UD1ItemFragment_Equippable* FromEquippable = FromItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable>();
	if (FromEquippable == nullptr)
		return false;
	
	if (FromEquippable->EquipmentType == EEquipmentType::Weapon)
	{
		const UD1ItemFragment_Equippable_Weapon* FromWeapon = Cast<UD1ItemFragment_Equippable_Weapon>(FromEquippable);
		EWeaponHandType FromWeaponHandType = FromWeapon->WeaponHandType;

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
	}
	else if (FromEquippable->EquipmentType == EEquipmentType::Armor)
	{
		const UD1ItemFragment_Equippable_Armor* EquippableArmor = Cast<UD1ItemFragment_Equippable_Armor>(FromEquippable);
		return IsSameArmorType(ToEquipmentSlotType, EquippableArmor->ArmorType);
	}
	return false;
}

bool UD1EquipmentManagerComponent::IsWeaponSlot(EEquipmentSlotType EquipmentSlotType)
{
	return (EquipmentSlotType == EEquipmentSlotType::Primary_LeftHand  || EquipmentSlotType == EEquipmentSlotType::Secondary_LeftHand  ||
			EquipmentSlotType == EEquipmentSlotType::Primary_RightHand || EquipmentSlotType == EEquipmentSlotType::Secondary_RightHand ||
			EquipmentSlotType == EEquipmentSlotType::Primary_TwoHand   || EquipmentSlotType == EEquipmentSlotType::Secondary_TwoHand);
}

bool UD1EquipmentManagerComponent::IsArmorSlot(EEquipmentSlotType EquipmentSlotType)
{
	return (EquipmentSlotType == EEquipmentSlotType::Helmet || EquipmentSlotType == EEquipmentSlotType::Chest ||
			EquipmentSlotType == EEquipmentSlotType::Legs   || EquipmentSlotType == EEquipmentSlotType::Hands ||
			EquipmentSlotType == EEquipmentSlotType::Foot);
}

bool UD1EquipmentManagerComponent::IsSameWeaponEquipState(EEquipmentSlotType EquipmentSlotType, EWeaponEquipState WeaponEquipState)
{
	return ((EquipmentSlotType == EEquipmentSlotType::Unarmed_LeftHand   || EquipmentSlotType == EEquipmentSlotType::Unarmed_RightHand) && WeaponEquipState == EWeaponEquipState::Unarmed ||
			(EquipmentSlotType == EEquipmentSlotType::Primary_LeftHand   || EquipmentSlotType == EEquipmentSlotType::Primary_RightHand   || EquipmentSlotType == EEquipmentSlotType::Primary_TwoHand)   && WeaponEquipState == EWeaponEquipState::Primary ||
		    (EquipmentSlotType == EEquipmentSlotType::Secondary_LeftHand || EquipmentSlotType == EEquipmentSlotType::Secondary_RightHand || EquipmentSlotType == EEquipmentSlotType::Secondary_TwoHand) && WeaponEquipState == EWeaponEquipState::Secondary);
}

bool UD1EquipmentManagerComponent::IsSameWeaponHandType(EEquipmentSlotType EquipmentSlotType, EWeaponHandType WeaponHandType)
{
	return ((EquipmentSlotType == EEquipmentSlotType::Primary_LeftHand  || EquipmentSlotType == EEquipmentSlotType::Secondary_LeftHand)  && WeaponHandType == EWeaponHandType::LeftHand  ||
			(EquipmentSlotType == EEquipmentSlotType::Primary_RightHand || EquipmentSlotType == EEquipmentSlotType::Secondary_RightHand) && WeaponHandType == EWeaponHandType::RightHand ||
			(EquipmentSlotType == EEquipmentSlotType::Primary_TwoHand   || EquipmentSlotType == EEquipmentSlotType::Secondary_TwoHand)   && WeaponHandType == EWeaponHandType::TwoHand);
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

AD1Player* UD1EquipmentManagerComponent::GetPlayerCharacter() const
{
	return Cast<AD1Player>(GetOwner());
}

AD1PlayerController* UD1EquipmentManagerComponent::GetPlayerController() const
{
	if (AD1Player* Character = GetPlayerCharacter())
	{
		return Cast<AD1PlayerController>(Character->Controller);
	}
	return nullptr;
}

const TArray<FD1EquipmentEntry>& UD1EquipmentManagerComponent::GetAllEntries() const
{
	return EquipmentList.GetAllEntries();
}

UD1AbilitySystemComponent* UD1EquipmentManagerComponent::GetAbilitySystemComponent() const
{
	return Cast<UD1AbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()));
}
