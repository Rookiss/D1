#include "D1EquipManagerComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "D1CosmeticManagerComponent.h"
#include "D1EquipmentManagerComponent.h"
#include "D1GameplayTags.h"
#include "Engine/ActorChannel.h"
#include "Item/D1ItemInstance.h"
#include "Item/Fragments/D1ItemFragment_Equippable.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Armor.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Weapon.h"
#include "Net/UnrealNetwork.h"
#include "Actors/D1WeaponBase.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Character/LyraCharacter.h"
#include "Player/LyraPlayerController.h"
#include "System/LyraAssetManager.h"
#include "System/LyraGameData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1EquipManagerComponent)

void FD1EquipEntry::Init(UD1ItemInstance* InItemInstance)
{
	if (ItemInstance == InItemInstance)
		return;

	ItemInstance = InItemInstance;
	ItemInstance ? Equip() : Unequip();
}

void FD1EquipEntry::Equip()
{
	if (ItemInstance == nullptr)
		return;

	ALyraCharacter* Character = EquipManager->GetCharacter();
	check(Character);
	
	const UD1ItemFragment_Equippable* EquippableFragment = ItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable>();
	check(EquippableFragment);

	if (EquipManager->GetOwner()->HasAuthority())
	{
		if (ULyraAbilitySystemComponent* ASC = Cast<ULyraAbilitySystemComponent>(EquipManager->GetAbilitySystemComponent()))
		{
			// Remove Previous Ability
			BaseAbilitySetHandles.TakeFromAbilitySystem(ASC);
	
			// Add Current Ability
			if (const ULyraAbilitySet* BaseAbilitySet = EquippableFragment->BaseAbilitySet)
			{
				BaseAbilitySet->GiveToAbilitySystem(ASC, &BaseAbilitySetHandles, ItemInstance);
			}

			// Remove Previous Stat
			ASC->RemoveActiveGameplayEffect(BaseStatHandle);
			BaseStatHandle.Invalidate();
	
			// Add Current Stat
			const TSubclassOf<UGameplayEffect> AttributeModifierGE = ULyraAssetManager::GetSubclassByPath(ULyraGameData::Get().AttributeModifierGameplayEffect);
			check(AttributeModifierGE);
		
			const FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
			const FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(AttributeModifierGE, 1.f, ContextHandle);

			TArray<FGameplayAttribute> OutAttributes;
			ASC->GetAllAttributes(OutAttributes);

			for (const FGameplayAttribute& Attribute : OutAttributes)
			{
				SpecHandle.Data->SetSetByCallerMagnitude(FName(*Attribute.GetName()), 0);
			}

			for (const FGameplayTagStack& Stack : ItemInstance->GetStatContainer().GetStacks())
			{
				SpecHandle.Data->SetSetByCallerMagnitude(Stack.GetStackTag(), Stack.GetStackCount());
			}
	
			BaseStatHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
		}
		
		// Weapon
		if (EquippableFragment->EquipmentType == EEquipmentType::Weapon)
		{
			// Despawn Previous Weapon
			if (IsValid(SpawnedWeaponActor))
			{
				SpawnedWeaponActor->Destroy();
			}

			// Spawn Current Weapon
			const UD1ItemFragment_Equippable_Weapon* WeaponFragment = ItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable_Weapon>();
			const FD1WeaponAttachInfo& AttachInfo = WeaponFragment->WeaponAttachInfo;
			if (AttachInfo.SpawnWeaponClass)
			{
				UWorld* World = EquipManager->GetWorld();
				AD1WeaponBase* NewWeaponActor = World->SpawnActorDeferred<AD1WeaponBase>(AttachInfo.SpawnWeaponClass, FTransform::Identity, Character);
				// NewWeaponActor->Init(ItemInstance->GetTemplateID(), EquipmentSlotType);
				NewWeaponActor->SetActorRelativeTransform(AttachInfo.AttachTransform);
				NewWeaponActor->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, AttachInfo.AttachSocket);
				NewWeaponActor->FinishSpawning(FTransform::Identity, true);
			}
		}
	}
	else
	{
		// Armor
		if (EquippableFragment->EquipmentType == EEquipmentType::Armor)
		{
			// Refresh Armor Mesh
			const UD1ItemFragment_Equippable_Armor* ArmorFragment = ItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable_Armor>();
			check(ArmorFragment);

			UD1CosmeticManagerComponent* CharacterCosmetics = Character->FindComponentByClass<UD1CosmeticManagerComponent>();
			check(CharacterCosmetics);
		
			CharacterCosmetics->SetArmorMesh(ArmorFragment->ArmorType, ArmorFragment->ArmorMesh);
		}
	}
}

void FD1EquipEntry::Unequip()
{
	if (EquipManager->GetOwner()->HasAuthority())
	{
		if (ULyraAbilitySystemComponent* ASC = Cast<ULyraAbilitySystemComponent>(EquipManager->GetAbilitySystemComponent()))
		{
			// Remove Ability
			BaseAbilitySetHandles.TakeFromAbilitySystem(ASC);

			// Remove Stat
			ASC->RemoveActiveGameplayEffect(BaseStatHandle);
			BaseStatHandle.Invalidate();
		}
		
		// Despawn Weapon
		if (UD1EquipmentManagerComponent::IsWeaponSlot(EquipmentSlotType))
		{
			if (IsValid(SpawnedWeaponActor))
			{
				SpawnedWeaponActor->Destroy();
			}
		}
	}
	
	// Refresh Armor Mesh
	if (UD1EquipmentManagerComponent::IsArmorSlot(EquipmentSlotType))
 	{
		ALyraCharacter* Character = EquipManager->GetCharacter();
		check(Character);
 
 		EArmorType ArmorType = EquipManager->ConvertToArmorType(EquipmentSlotType);

		UD1CosmeticManagerComponent* CharacterCosmetics = Character->FindComponentByClass<UD1CosmeticManagerComponent>();
		check(CharacterCosmetics);

		CharacterCosmetics->SetArmorMesh(ArmorType, nullptr);
 	}
 }

bool FD1EquipList::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
{
	return FFastArraySerializer::FastArrayDeltaSerialize<FD1EquipEntry, FD1EquipList>(Entries, DeltaParams,*this);
}

void FD1EquipList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		FD1EquipEntry& Entry = Entries[Index];
		Entry.EquipManager = EquipManager;
		Entry.EquipmentSlotType = (EEquipmentSlotType)Index;
		
		if (Entry.ItemInstance)
		{
			Entry.Equip();
		}
	}
}

void FD1EquipList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		FD1EquipEntry& Entry = Entries[Index];
		Entry.ItemInstance ? Entry.Equip() : Entry.Unequip();
	}
}

void FD1EquipList::Equip(EEquipmentSlotType EquipmentSlotType, UD1ItemInstance* ItemInstance)
{
	FD1EquipEntry& Entry = Entries[(int32)(EquipmentSlotType)];
	Entry.Init(ItemInstance);
	MarkItemDirty(Entry);
}

void FD1EquipList::Unequip(EEquipmentSlotType EquipmentSlotType)
{
	FD1EquipEntry& Entry = Entries[(int32)(EquipmentSlotType)];
	Entry.Init(nullptr);
	MarkItemDirty(Entry);
}

UD1EquipManagerComponent::UD1EquipManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, EquipList(this)
{
    SetIsReplicatedByDefault(true);
	bWantsInitializeComponent = true;
}

void UD1EquipManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		TArray<FD1EquipEntry>& Entries = EquipList.Entries;
		Entries.SetNum((int32)EEquipmentSlotType::Count);

		for (int32 i = 0; i < Entries.Num(); i++)
		{
			FD1EquipEntry& Entry = Entries[i];
			Entry.EquipManager = this;
			Entry.EquipmentSlotType = (EEquipmentSlotType)i;
			EquipList.MarkItemDirty(Entry);
		}
	}
}

void UD1EquipManagerComponent::UninitializeComponent()
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		for (int32 i = 0; i < (int32)EEquipmentSlotType::Count; i++)
		{
			Unequip((EEquipmentSlotType)i);
		}
	}
	
	Super::UninitializeComponent();
}

void UD1EquipManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EquipList);
	DOREPLIFETIME(ThisClass, CurrentWeaponEquipState);
}

bool UD1EquipManagerComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	
	for (FD1EquipEntry& Entry : EquipList.Entries)
	{
		UD1ItemInstance* ItemInstance = Entry.ItemInstance;
		if (IsValid(ItemInstance))
		{
			bWroteSomething |= Channel->ReplicateSubobject(ItemInstance, *Bunch, *RepFlags);
		}
	}
	
	return bWroteSomething;
}

void UD1EquipManagerComponent::Equip(EEquipmentSlotType EquipmentSlotType, UD1ItemInstance* ItemInstance)
{
	check(GetOwner()->HasAuthority());

	if (EquipmentSlotType == EEquipmentSlotType::Count || ItemInstance == nullptr)
		return;
	
	EquipList.Equip(EquipmentSlotType, ItemInstance);
}

void UD1EquipManagerComponent::Unequip(EEquipmentSlotType EquipmentSlotType)
{
	check(GetOwner()->HasAuthority())

	if (EquipmentSlotType == EEquipmentSlotType::Count)
		return;

	EquipList.Unequip(EquipmentSlotType);
}

void UD1EquipManagerComponent::EquipWeaponInSlot()
{
	check(GetOwner()->HasAuthority());

	if (CurrentWeaponEquipState == EWeaponEquipState::Count)
		return;
	
	UD1EquipmentManagerComponent* EquipmentManager = GetEquipmentManagerComponent();
	check(EquipmentManager);

	const TArray<FD1EquipmentEntry>& Entries = EquipmentManager->GetAllEntries();
	for (EEquipmentSlotType EquipmentSlotType : Item::EquipmentSlotsByWeaponState[(int32)CurrentWeaponEquipState])
	{
		Equip(EquipmentSlotType, Entries[(int32)EquipmentSlotType].GetItemInstance());
	}
}

void UD1EquipManagerComponent::UnequipWeaponInSlot()
{
	check(GetOwner()->HasAuthority());

	if (CurrentWeaponEquipState == EWeaponEquipState::Count)
		return;
	
	UD1EquipmentManagerComponent* EquipmentManager = GetEquipmentManagerComponent();
	check(EquipmentManager);
	
	for (EEquipmentSlotType EquipmentSlotType : Item::EquipmentSlotsByWeaponState[(int32)CurrentWeaponEquipState])
	{
		Unequip(EquipmentSlotType);
	}
}

void UD1EquipManagerComponent::ChangeWeaponEquipState(EWeaponEquipState NewWeaponEquipState)
{
	check(GetOwner()->HasAuthority());

	if (CanChangeWeaponEquipState(NewWeaponEquipState))
	{
		if (CurrentWeaponEquipState == NewWeaponEquipState)
		{
			NewWeaponEquipState = EWeaponEquipState::Unarmed;
		}
		
		UnequipWeaponInSlot();
		CurrentWeaponEquipState = NewWeaponEquipState;
		EquipWeaponInSlot();
	}
}

bool UD1EquipManagerComponent::CanChangeWeaponEquipState(EWeaponEquipState NewWeaponEquipState) const
{
	if (NewWeaponEquipState == EWeaponEquipState::Count)
		return false;

	UD1EquipmentManagerComponent* EquipmentManager = GetEquipmentManagerComponent();
	check(EquipmentManager);
	
	return (EquipmentManager->IsAllEmpty(NewWeaponEquipState) == false);
}

AD1WeaponBase* UD1EquipManagerComponent::GetEquippedWeapon(EWeaponHandType WeaponHandType) const
{
	if (WeaponHandType == EWeaponHandType::Count)
		return nullptr;

	const TArray<FD1EquipEntry>& Entries = EquipList.Entries;
	int32 EntryIndex = (int32)ConvertToEquipmentSlotType(WeaponHandType);
	return Entries.IsValidIndex(EntryIndex) ? Entries[EntryIndex].SpawnedWeaponActor : nullptr;
}

AD1WeaponBase* UD1EquipManagerComponent::GetFirstEquippedWeapon() const
{
	const TArray<FD1EquipEntry>& Entries = EquipList.Entries;
	
	AD1WeaponBase* WeaponActor = nullptr;
	for (int i = 0; i < (int32)EWeaponHandType::Count; i++)
	{
		int32 EntryIndex = (int32)ConvertToEquipmentSlotType((EWeaponHandType)i);
		if (Entries.IsValidIndex(EntryIndex) == false)
			continue;

		WeaponActor = Entries[EntryIndex].SpawnedWeaponActor;
		if (WeaponActor)
			break;
	}
	return WeaponActor;
}

void UD1EquipManagerComponent::OnRep_CurrentWeaponEquipState()
{
	BroadcastChangedMessgae(CurrentWeaponEquipState);
}

void UD1EquipManagerComponent::BroadcastChangedMessgae(EWeaponEquipState NewWeaponEquipState)
{
	FD1WeaponEquipStateChangedMessage Message;
	Message.WeaponEquipState = NewWeaponEquipState;

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());
	MessageSubsystem.BroadcastMessage(D1GameplayTags::Message_Item_WeaponEquipStateChanged, Message);
}

ALyraCharacter* UD1EquipManagerComponent::GetCharacter() const
{
	return Cast<ALyraCharacter>(GetOwner());
}

ALyraPlayerController* UD1EquipManagerComponent::GetPlayerController() const
{
	if (ALyraCharacter* Character = GetCharacter())
	{
		return Cast<ALyraPlayerController>(Character->Controller);
	}
	return nullptr;
}

TArray<FD1EquipEntry>& UD1EquipManagerComponent::GetAllEntries()
{
	return EquipList.GetAllEntries();
}

UAbilitySystemComponent* UD1EquipManagerComponent::GetAbilitySystemComponent() const
{
	return UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
}

UD1EquipmentManagerComponent* UD1EquipManagerComponent::GetEquipmentManagerComponent() const
{
	if (ALyraPlayerController* PlayerController = GetController<ALyraPlayerController>())
	{
		return PlayerController->FindComponentByClass<UD1EquipmentManagerComponent>();
	}
	return nullptr;
}

EWeaponHandType UD1EquipManagerComponent::ConvertToWeaponHandType(EEquipmentSlotType EquipmentSlotType) const
{
	EWeaponHandType WeaponHandType = EWeaponHandType::Count;
	
	switch (EquipmentSlotType)
	{
	case EEquipmentSlotType::Unarmed_LeftHand:
	case EEquipmentSlotType::Primary_LeftHand:
	case EEquipmentSlotType::Secondary_LeftHand:
	case EEquipmentSlotType::Tertiary_LeftHand:
	case EEquipmentSlotType::Quaternary_LeftHand:
		WeaponHandType = EWeaponHandType::LeftHand;
		break;
	case EEquipmentSlotType::Unarmed_RightHand:
	case EEquipmentSlotType::Primary_RightHand:
	case EEquipmentSlotType::Secondary_RightHand:
	case EEquipmentSlotType::Tertiary_RightHand:
	case EEquipmentSlotType::Quaternary_RightHand:
		WeaponHandType = EWeaponHandType::RightHand;
		break;
	case EEquipmentSlotType::Primary_TwoHand:
	case EEquipmentSlotType::Secondary_TwoHand:
	case EEquipmentSlotType::Tertiary_TwoHand:
	case EEquipmentSlotType::Quaternary_TwoHand:
		WeaponHandType = EWeaponHandType::TwoHand;
		break;
	}

	return WeaponHandType;
}

EEquipmentSlotType UD1EquipManagerComponent::ConvertToEquipmentSlotType(EWeaponHandType WeaponHandType) const
{
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;

	if (CurrentWeaponEquipState == EWeaponEquipState::Unarmed)
	{
		switch (WeaponHandType)
		{
		case EWeaponHandType::LeftHand:  EquipmentSlotType = EEquipmentSlotType::Unarmed_LeftHand;  break;
		case EWeaponHandType::RightHand: EquipmentSlotType = EEquipmentSlotType::Unarmed_RightHand; break;
		}
	}
	else if (CurrentWeaponEquipState == EWeaponEquipState::Primary)
	{
		switch (WeaponHandType)
		{
		case EWeaponHandType::LeftHand:  EquipmentSlotType = EEquipmentSlotType::Primary_LeftHand;  break;
		case EWeaponHandType::RightHand: EquipmentSlotType = EEquipmentSlotType::Primary_RightHand; break;
		case EWeaponHandType::TwoHand:   EquipmentSlotType = EEquipmentSlotType::Primary_TwoHand;   break;
		}
	}
	else if (CurrentWeaponEquipState == EWeaponEquipState::Secondary)
	{
		switch (WeaponHandType)
		{
		case EWeaponHandType::LeftHand:  EquipmentSlotType = EEquipmentSlotType::Secondary_LeftHand;  break;
		case EWeaponHandType::RightHand: EquipmentSlotType = EEquipmentSlotType::Secondary_RightHand; break;
		case EWeaponHandType::TwoHand:   EquipmentSlotType = EEquipmentSlotType::Secondary_TwoHand;   break;
		}
	}
	else if (CurrentWeaponEquipState == EWeaponEquipState::Tertiary)
	{
		switch (WeaponHandType)
		{
		case EWeaponHandType::LeftHand:  EquipmentSlotType = EEquipmentSlotType::Tertiary_LeftHand;  break;
		case EWeaponHandType::RightHand: EquipmentSlotType = EEquipmentSlotType::Tertiary_RightHand; break;
		case EWeaponHandType::TwoHand:   EquipmentSlotType = EEquipmentSlotType::Tertiary_TwoHand;   break;
		}
	}
	else if (CurrentWeaponEquipState == EWeaponEquipState::Quaternary)
	{
		switch (WeaponHandType)
		{
		case EWeaponHandType::LeftHand:  EquipmentSlotType = EEquipmentSlotType::Quaternary_LeftHand;  break;
		case EWeaponHandType::RightHand: EquipmentSlotType = EEquipmentSlotType::Quaternary_RightHand; break;
		case EWeaponHandType::TwoHand:   EquipmentSlotType = EEquipmentSlotType::Quaternary_TwoHand;   break;
		}
	}
	
	return EquipmentSlotType;
}

EArmorType UD1EquipManagerComponent::ConvertToArmorType(EEquipmentSlotType EquipmentSlotType) const
{
	EArmorType ArmorType = EArmorType::Count;
	
	switch (EquipmentSlotType)
	{
	case EEquipmentSlotType::Helmet:	ArmorType = EArmorType::Helmet;	break;
	case EEquipmentSlotType::Chest:		ArmorType = EArmorType::Chest;	break;
	case EEquipmentSlotType::Legs:		ArmorType = EArmorType::Legs;	break;
	case EEquipmentSlotType::Hands:		ArmorType = EArmorType::Hands;	break;
	case EEquipmentSlotType::Foot:		ArmorType = EArmorType::Foot;	break;
	}

	return ArmorType;
}
