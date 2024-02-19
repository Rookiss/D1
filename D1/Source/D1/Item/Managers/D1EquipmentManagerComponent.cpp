#include "D1EquipmentManagerComponent.h"

#include "AbilitySystemGlobals.h"
#include "D1InventoryManagerComponent.h"
#include "AbilitySystem/D1AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/D1PlayerSet.h"
#include "Character/D1Player.h"
#include "Engine/ActorChannel.h"
#include "GameFramework/Character.h"
#include "Item/D1ItemInstance.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Armor.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Weapon.h"
#include "Net/UnrealNetwork.h"
#include "System/D1AssetManager.h"
#include "Weapon/D1WeaponBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1EquipmentManagerComponent)

void FD1EquipmentEntry::Init(UD1ItemInstance* NewItemInstance)
{
	if (ItemInstance)
	{
		Unequip();
	}

	ItemInstance = NewItemInstance;
	if (ItemInstance)
	{
		LastValidItemID = ItemInstance->ItemID;
	}

	if (ItemInstance)
	{
		const UD1ItemFragment_Equippable* Equippable = ItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable>();
		check(Equippable);
		
		if (Equippable->EquipmentType == EEquipmentType::Weapon)
		{
			EWeaponSlotType CurrentWeaponSlotType = EquipmentManagerComponent->GetCurrentWeaponSlotType();
			if (EquipmentManagerComponent->IsSameWeaponSlotType(EquipmentSlotType, CurrentWeaponSlotType))
			{
				Equip();
			}
		}
		else if (Equippable->EquipmentType == EEquipmentType::Armor)
		{
			Equip();
		}
	}
}

void FD1EquipmentEntry::Equip()
{
	if (ItemInstance == nullptr)
		return;

	const UD1ItemFragment_Equippable* Equippable = ItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable>();
	check(Equippable);

	EWeaponSlotType CurrentWeaponSlotType = EquipmentManagerComponent->GetCurrentWeaponSlotType();
	if (Equippable->EquipmentType == EEquipmentType::Weapon && EquipmentManagerComponent->IsSameWeaponSlotType(EquipmentSlotType, CurrentWeaponSlotType) == false)
		return;
	
	UD1AbilitySystemComponent* ASC = Cast<UD1AbilitySystemComponent>(EquipmentManagerComponent->GetAbilitySystemComponent());
	check(ASC);

	Unequip();
	
	// Ability
	if (const UD1AbilitySystemData* AbilitySystemData = Equippable->AbilitySystemDataToGrant)
	{
		AbilitySystemData->GiveToAbilitySystem(ASC, &GrantedHandles, ItemInstance);
	}

	// Stat
	const FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	const FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(UD1AssetManager::GetSubclassByName<UGameplayEffect>("AttributeModifier"), 1.f, ContextHandle);
	
	if (const UD1MonsterSet* MonsterSet = Cast<UD1MonsterSet>(ASC->GetAttributeSet(UD1MonsterSet::StaticClass())))
	{
		TArray<FGameplayTag> AttributeTags;
		MonsterSet->GetAllAttributeTags(AttributeTags);
		for (const FGameplayTag& Tag : AttributeTags)
		{
			SpecHandle.Data->SetSetByCallerMagnitude(Tag, 0);
		}
	}
	
	if (const UD1PlayerSet* PlayerSet = Cast<UD1PlayerSet>(ASC->GetAttributeSet(UD1PlayerSet::StaticClass())))
	{
		TArray<FGameplayTag> AttributeTags;
		PlayerSet->GetAllAttributeTags(AttributeTags);
		for (const FGameplayTag& Tag : AttributeTags)
		{
			SpecHandle.Data->SetSetByCallerMagnitude(Tag, 0);
		}
	}

	for (const FD1GameplayTagStack& Stack : ItemInstance->GetStatContainer().GetStacks())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(Stack.GetStatTag(), Stack.GetStatCount());
	}
	
	StatHandles.Add(ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data));

	// Visual
	if (Equippable->EquipmentType == EEquipmentType::Weapon)
	{
		const UD1ItemFragment_Equippable_Weapon* Weapon = ItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable_Weapon>();
		const FD1WeaponAttachInfo& AttachInfo = Weapon->WeaponAttachInfo;
		if (AttachInfo.SpawnWeaponClass)
		{
			APawn* OwningPawn = Cast<APawn>(EquipmentManagerComponent->GetOwner());
			check(OwningPawn);
		
			USceneComponent* AttachTarget = OwningPawn->GetRootComponent();
			if (ACharacter* OwningCharacter = Cast<ACharacter>(OwningPawn))
			{
				AttachTarget = OwningCharacter->GetMesh();
			}
		
			UWorld* World = EquipmentManagerComponent->GetWorld();
			AD1WeaponBase* NewActor = World->SpawnActorDeferred<AD1WeaponBase>(AttachInfo.SpawnWeaponClass, FTransform::Identity, OwningPawn);
			NewActor->FinishSpawning(FTransform::Identity, true);
			NewActor->SetActorRelativeTransform(AttachInfo.AttachTransform);
			NewActor->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform, AttachInfo.AttachSocket);
		
			SpawnedWeaponActor = NewActor;
		}
	}
	else if (Equippable->EquipmentType == EEquipmentType::Armor)
	{
		const UD1ItemFragment_Equippable_Armor* Armor = ItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable_Armor>();
		AD1Player* Player = Cast<AD1Player>(EquipmentManagerComponent->GetOwner());
		Player->Multicast_SetArmorMesh(Armor->ArmorType, Armor->ArmorMesh.ToSoftObjectPath());
	}
}

void FD1EquipmentEntry::Unequip()
{
	if (ItemInstance == nullptr)
		return;

	UD1AbilitySystemComponent* ASC = Cast<UD1AbilitySystemComponent>(EquipmentManagerComponent->GetAbilitySystemComponent());
	check(ASC);
	
	// Ability
	GrantedHandles.TakeFromAbilitySystem(ASC);

	// Stat
	for (const FActiveGameplayEffectHandle& Handle : StatHandles)
	{
		ASC->RemoveActiveGameplayEffect(Handle);
	}
	StatHandles.Reset();

	// Visual
	const UD1ItemFragment_Equippable* Equippable = ItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable>();
	if (Equippable->EquipmentType == EEquipmentType::Weapon)
	{
		if (IsValid(SpawnedWeaponActor))
		{
			SpawnedWeaponActor->Destroy();
		}
	}
	else if (Equippable->EquipmentType == EEquipmentType::Armor)
	{
		const UD1ItemFragment_Equippable_Armor* Armor = ItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable_Armor>();
		AD1Player* Player = Cast<AD1Player>(EquipmentManagerComponent->GetOwner());
		Player->Multicast_SetArmorMesh(Armor->ArmorType, FSoftObjectPath());
	}
}

FString FD1EquipmentEntry::GetDebugString() const
{
	return FString::Printf(TEXT("[%s]"), *ItemInstance->GetDebugString());
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
		Entry.EquipmentManagerComponent = EquipmentManagerComponent;
		Entry.EquipmentSlotType = static_cast<EEquipmentSlotType>(Index);
		
		UD1ItemInstance* ItemInstance = Entry.GetItemInstance();
		if (ItemInstance == nullptr)
			continue;
		
		if (EquipmentManagerComponent->OnEquipmentEntryChanged.IsBound())
		{
			EquipmentManagerComponent->OnEquipmentEntryChanged.Broadcast(static_cast<EEquipmentSlotType>(Index), ItemInstance);
		}
	}
}

void FD1EquipmentList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		const FD1EquipmentEntry& Entry = Entries[Index];
		
		if (EquipmentManagerComponent->OnEquipmentEntryChanged.IsBound())
		{
			EquipmentManagerComponent->OnEquipmentEntryChanged.Broadcast(static_cast<EEquipmentSlotType>(Index), Entry.ItemInstance);
		}
	}
}

void FD1EquipmentList::InitOnServer()
{
	Entries.SetNum(EquipmentSlotCount);

	for (int32 i = 0; i < Entries.Num(); i++)
	{
		FD1EquipmentEntry& Entry = Entries[i];
		Entry.EquipmentManagerComponent = EquipmentManagerComponent;
		Entry.EquipmentSlotType = static_cast<EEquipmentSlotType>(i);
		MarkItemDirty(Entry);
	}
}

void FD1EquipmentList::SetEntry_Unsafe(EEquipmentSlotType EquipmentSlotType, UD1ItemInstance* ItemInstance)
{
	FD1EquipmentEntry& Entry = Entries[EquipmentSlotType];
	Entry.SetEntry(ItemInstance);
	MarkItemDirty(Entry);
}

UD1ItemInstance* FD1EquipmentList::ResetEntry_Unsafe(EEquipmentSlotType EquipmentSlotType)
{
	FD1EquipmentEntry& Entry = Entries[EquipmentSlotType];
	UD1ItemInstance* ItemInstance = Entry.ItemInstance;
	Entry.SetEntry(nullptr);
	MarkItemDirty(Entry);
	return ItemInstance;
}

void FD1EquipmentList::Equip_Unsafe(EEquipmentSlotType EquipmentSlotType)
{
	Entries[EquipmentSlotType].Equip();
}

UD1ItemInstance* FD1EquipmentList::Unequip_Unsafe(EEquipmentSlotType EquipmentSlotType)
{
	return nullptr;
}

void FD1EquipmentList::SetWeaponSlotType_Unsafe(EWeaponSlotType PrevWeaponSlotType, EWeaponSlotType NewWeaponSlotType)
{
	TArray<EEquipmentSlotType> SlotTypes;

	// Prev - Unequip
	if (PrevWeaponSlotType == EWeaponSlotType::Primary)
	{
		SlotTypes = { Primary_LeftHand, Primary_RightHand, Primary_TwoHand };
	}
	else if (PrevWeaponSlotType == EWeaponSlotType::Secondary)
	{
		SlotTypes = { Secondary_LeftHand, Secondary_RightHand, Secondary_TwoHand };
	}

	for (EEquipmentSlotType SlotType : SlotTypes)
	{
		FD1EquipmentEntry& Entry = Entries[static_cast<int32>(SlotType)];
		Entry.Unequip();
	}

	// New - Equip
	if (NewWeaponSlotType == EWeaponSlotType::Primary)
	{
		SlotTypes = { Primary_LeftHand, Primary_RightHand, Primary_TwoHand };
	}
	else if (NewWeaponSlotType == EWeaponSlotType::Secondary)
	{
		SlotTypes = { Secondary_LeftHand, Secondary_RightHand, Secondary_TwoHand };
	}

	for (EEquipmentSlotType SlotType : SlotTypes)
	{
		FD1EquipmentEntry& Entry = Entries[static_cast<int32>(SlotType)];
		Entry.Equip();
	}
}

FD1EquipmentEntry FD1EquipmentList::GetEntryBySlotType(EEquipmentSlotType EquipmentSlotType) const
{
	return Entries[EquipmentSlotType];
}

UD1EquipmentManagerComponent::UD1EquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, EquipmentList(this)
{
    SetIsReplicatedByDefault(true);
	bWantsInitializeComponent = true;
}

void UD1EquipmentManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		EquipmentList.InitOnServer();
	}
}

void UD1EquipmentManagerComponent::UninitializeComponent()
{
	for (FD1EquipmentEntry& Entry : EquipmentList.Entries)
	{
		Entry.Reset();
	}
	
	Super::UninitializeComponent();
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

void UD1EquipmentManagerComponent::Server_RequestEquipItem_FromInventory_Implementation(UD1InventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, EEquipmentSlotType ToEquipmentSlotType)
{
	check(GetOwner()->HasAuthority());

	if (CanEquipItem_FromInventory(OtherComponent, FromItemSlotPos, ToEquipmentSlotType) == false)
		return;
	
	UD1ItemInstance* RemovedItemInstance = OtherComponent->InventoryList.RemoveItem_Unsafe(FromItemSlotPos, 1);
	EquipmentList.EquipItem_Unsafe(ToEquipmentSlotType, RemovedItemInstance);
}

bool UD1EquipmentManagerComponent::CanEquipItem_FromInventory(UD1InventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, EEquipmentSlotType ToEquipmentSlotType) const
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

	return CanEquipItem(FromItemInstance, ToEquipmentSlotType);
}

void UD1EquipmentManagerComponent::Server_RequestEquipItem_FromEquipment_Implementation(UD1EquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, EEquipmentSlotType ToEquipmentSlotType)
{
	check(GetOwner()->HasAuthority());

	if (CanEquipItem_FromEquipment(OtherComponent, FromEquipmentSlotType, ToEquipmentSlotType) == false)
		return;

	UD1ItemInstance* RemovedItemInstance = OtherComponent->EquipmentList.UnequipItem_Unsafe(FromEquipmentSlotType);
	EquipmentList.EquipItem_Unsafe(ToEquipmentSlotType, RemovedItemInstance);
}

bool UD1EquipmentManagerComponent::CanEquipItem_FromEquipment(UD1EquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, EEquipmentSlotType ToEquipmentSlotType) const
{
	if (OtherComponent == nullptr)
		return false;
	
	if (FromEquipmentSlotType == EEquipmentSlotType::EquipmentSlotCount || ToEquipmentSlotType == EEquipmentSlotType::EquipmentSlotCount)
		return false;
	
	const TArray<FD1EquipmentEntry>& FromEntries = OtherComponent->GetAllEntries();
	const FD1EquipmentEntry& FromEntry = FromEntries[FromEquipmentSlotType];
	UD1ItemInstance* FromItemInstance = FromEntry.GetItemInstance();

	return CanEquipItem(FromItemInstance, ToEquipmentSlotType);
}

bool UD1EquipmentManagerComponent::CanEquipItem(UD1ItemInstance* FromItemInstance, EEquipmentSlotType ToEquipmentSlotType) const
{
	if (ToEquipmentSlotType == EEquipmentSlotType::EquipmentSlotCount || FromItemInstance == nullptr)
		return false;

	const TArray<FD1EquipmentEntry>& Entries = GetAllEntries();
	const FD1EquipmentEntry& Entry = Entries[ToEquipmentSlotType];
	if (FromItemInstance == Entry.ItemInstance)
		return true;

	if (Entry.ItemInstance)
		return false;
	
	const UD1ItemFragment_Equippable* Equippable = FromItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable>();
	if (Equippable == nullptr)
		return false;
	
	if (Equippable->EquipmentType == EEquipmentType::Weapon)
	{
		const UD1ItemFragment_Equippable_Weapon* EquippableWeapon = Cast<UD1ItemFragment_Equippable_Weapon>(Equippable);
		EWeaponHandType WeaponHandType = EquippableWeapon->WeaponHandType;

		if (IsSameWeaponHandType(ToEquipmentSlotType, WeaponHandType) == false)
			return false;
		
		if (IsPrimaryWeaponSlotType(ToEquipmentSlotType))
		{
			if (WeaponHandType == EWeaponHandType::LeftHand || WeaponHandType == EWeaponHandType::RightHand)
			{
				const FD1EquipmentEntry& TwoHandEntry = Entries[Primary_TwoHand];
				return (TwoHandEntry.ItemInstance == nullptr);
			}
			else if (WeaponHandType == EWeaponHandType::TwoHand)
			{
				const FD1EquipmentEntry& LeftHandEntry = Entries[Primary_LeftHand];
				const FD1EquipmentEntry& RightHandEntry = Entries[Primary_RightHand];
				return (LeftHandEntry.ItemInstance == nullptr && RightHandEntry.ItemInstance == nullptr);
			}
		}
		else if (IsSecondaryWeaponSlotType(ToEquipmentSlotType))
		{
			if (WeaponHandType == EWeaponHandType::LeftHand || WeaponHandType == EWeaponHandType::RightHand)
			{
				const FD1EquipmentEntry& TwoHandEntry = Entries[Secondary_TwoHand];
				return (TwoHandEntry.ItemInstance == nullptr);
			}
			else if (WeaponHandType == EWeaponHandType::TwoHand)
			{
				const FD1EquipmentEntry& LeftHandEntry = Entries[Secondary_LeftHand];
				const FD1EquipmentEntry& RightHandEntry = Entries[Secondary_RightHand];
				return (LeftHandEntry.ItemInstance == nullptr && RightHandEntry.ItemInstance == nullptr);
			}
		}
	}
	else if (Equippable->EquipmentType == EEquipmentType::Armor)
	{
		const UD1ItemFragment_Equippable_Armor* EquippableArmor = Cast<UD1ItemFragment_Equippable_Armor>(Equippable);
		return IsSameArmorType(ToEquipmentSlotType, EquippableArmor->ArmorType);
	}
	return false;
}

void UD1EquipmentManagerComponent::Server_RequestSetWeaponSlotType_Implementation(EWeaponSlotType NewWeaponSlotType)
{
	check(GetOwner()->HasAuthority());
	
	if (NewWeaponSlotType == EWeaponSlotType::Count || CurrentWeaponSlotType == NewWeaponSlotType)
		return;

	EquipmentList.SetWeaponSlotType_Unsafe(NewWeaponSlotType);
	CurrentWeaponSlotType = NewWeaponSlotType;
}

void UD1EquipmentManagerComponent::CycleWeaponSlotForward()
{
	Cur
}

void UD1EquipmentManagerComponent::CycleWeaponSlotBackward()
{
	
}

bool UD1EquipmentManagerComponent::IsSameWeaponHandType(EEquipmentSlotType EquipmentSlotType, EWeaponHandType WeaponHandType) const
{
	return ((EquipmentSlotType == EEquipmentSlotType::Primary_LeftHand  || EquipmentSlotType == EEquipmentSlotType::Secondary_LeftHand)  && WeaponHandType == EWeaponHandType::LeftHand  ||
			(EquipmentSlotType == EEquipmentSlotType::Primary_RightHand || EquipmentSlotType == EEquipmentSlotType::Secondary_RightHand) && WeaponHandType == EWeaponHandType::RightHand ||
			(EquipmentSlotType == EEquipmentSlotType::Primary_TwoHand   || EquipmentSlotType == EEquipmentSlotType::Secondary_TwoHand)   && WeaponHandType == EWeaponHandType::TwoHand);
}

bool UD1EquipmentManagerComponent::IsSameArmorType(EEquipmentSlotType EquipmentSlotType, EArmorType ArmorType) const
{
	return (EquipmentSlotType == EEquipmentSlotType::Helmet && ArmorType == EArmorType::Helmet || EquipmentSlotType == EEquipmentSlotType::Chest && ArmorType == EArmorType::Chest ||
			EquipmentSlotType == EEquipmentSlotType::Legs   && ArmorType == EArmorType::Legs   || EquipmentSlotType == EEquipmentSlotType::Hands && ArmorType == EArmorType::Hands ||
			EquipmentSlotType == EEquipmentSlotType::Foot   && ArmorType == EArmorType::Foot);
}

bool UD1EquipmentManagerComponent::IsPrimaryWeaponSlotType(EEquipmentSlotType EquipmentSlotType) const
{
	return (EquipmentSlotType == EEquipmentSlotType::Primary_LeftHand || EquipmentSlotType == EEquipmentSlotType::Primary_RightHand || EquipmentSlotType == EEquipmentSlotType::Primary_TwoHand);
}

bool UD1EquipmentManagerComponent::IsSecondaryWeaponSlotType(EEquipmentSlotType EquipmentSlotType) const
{
	return (EquipmentSlotType == EEquipmentSlotType::Secondary_LeftHand || EquipmentSlotType == EEquipmentSlotType::Secondary_RightHand || EquipmentSlotType == EEquipmentSlotType::Secondary_TwoHand);
}

bool UD1EquipmentManagerComponent::IsSameWeaponSlotType(EEquipmentSlotType EquipmentSlotType, EWeaponSlotType WeaponSlotType) const
{
	return ((EquipmentSlotType == Primary_LeftHand   || EquipmentSlotType == EEquipmentSlotType::Primary_RightHand   || EquipmentSlotType == EEquipmentSlotType::Primary_TwoHand)   && WeaponSlotType == EWeaponSlotType::Primary ||
		    (EquipmentSlotType == Secondary_LeftHand || EquipmentSlotType == EEquipmentSlotType::Secondary_RightHand || EquipmentSlotType == EEquipmentSlotType::Secondary_TwoHand) && WeaponSlotType == EWeaponSlotType::Secondary);
}

const TArray<FD1EquipmentEntry>& UD1EquipmentManagerComponent::GetAllEntries() const
{
	return EquipmentList.GetAllEntries();
}

UD1AbilitySystemComponent* UD1EquipmentManagerComponent::GetAbilitySystemComponent() const
{
	return Cast<UD1AbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()));
}
