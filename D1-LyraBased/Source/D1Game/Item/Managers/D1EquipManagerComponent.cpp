#include "D1EquipManagerComponent.h"

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
#include "AbilitySystem/Attributes/LyraCombatSet.h"
#include "Actors/D1CosmeticWeapon.h"
#include "Character/LyraCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/LyraPlayerController.h"
#include "PocketWorld/D1PocketStage.h"
#include "PocketWorld/D1PocketWorldSubsystem.h"
#include "System/D1GameplayTagStack.h"
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
			const TSharedPtr<FGameplayEffectSpec>& SpecData = SpecHandle.Data;
			
			for (const FGameplayModifierInfo& ModifierInfo : SpecData->Def->Modifiers)
			{
				SpecData->SetSetByCallerMagnitude(ModifierInfo.ModifierMagnitude.GetSetByCallerFloat().DataTag, 0);
			}
			
			for (const FD1GameplayTagStack& Stack : ItemInstance->GetStatContainer().GetStacks())
			{
				SpecData->SetSetByCallerMagnitude(Stack.GetStackTag(), Stack.GetStackCount());
			}
			
			BaseStatHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
		}
		
		// Weapon
		if (EquippableFragment->EquipmentType == EEquipmentType::Weapon)
		{
			// Despawn Previous Real Weapon
			if (IsValid(SpawnedWeaponActor))
			{
				SpawnedWeaponActor->Destroy();
			}

			// Spawn Current Real Weapon
			const UD1ItemFragment_Equippable_Weapon* WeaponFragment = ItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable_Weapon>();
			const FD1WeaponAttachInfo& AttachInfo = WeaponFragment->WeaponAttachInfo;
			if (AttachInfo.SpawnWeaponClass)
			{
				UWorld* World = EquipManager->GetWorld();
				AD1WeaponBase* NewWeaponActor = World->SpawnActorDeferred<AD1WeaponBase>(AttachInfo.SpawnWeaponClass, FTransform::Identity, Character);
				NewWeaponActor->Init(ItemInstance->GetItemTemplateID(), EquipmentSlotType);
				NewWeaponActor->SetActorRelativeTransform(AttachInfo.AttachTransform);
				NewWeaponActor->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, AttachInfo.AttachSocket);
				NewWeaponActor->FinishSpawning(FTransform::Identity, true);
			}
		}
	}
	else
	{
		if (EquippableFragment->EquipmentType == EEquipmentType::Weapon)
		{
			if (Character->IsLocallyControlled())
			{
				// Despawn Previous Pocket Weapon
				if (IsValid(SpawnedCosmeticWeapon))
				{
					SpawnedCosmeticWeapon->Destroy();
				}

				// Spawn Current Pocket Weapon
				const UD1ItemFragment_Equippable_Weapon* WeaponFragment = ItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable_Weapon>();

				if (UD1PocketWorldSubsystem* PocketWorldSubsystem = EquipManager->GetWorld()->GetSubsystem<UD1PocketWorldSubsystem>())
				{
					if (APlayerController* PC = Character->GetLyraPlayerController())
					{
						PocketWorldSubsystem->RegisterAndCallForGetPocketStage(PC->GetLocalPlayer(),
							FGetPocketStageDelegate::CreateLambda([this, WeaponFragment](AD1PocketStage* PocketStage)
							{
								if (IsValid(PocketStage))
								{
									ACharacter* Character = PocketStage->GetCharacter();
									const FD1WeaponAttachInfo& AttachInfo = WeaponFragment->WeaponAttachInfo;
									const AD1WeaponBase* WeaponCDO = AttachInfo.SpawnWeaponClass->GetDefaultObject<AD1WeaponBase>();
									
									UWorld* World = EquipManager->GetWorld();
									SpawnedCosmeticWeapon = World->SpawnActorDeferred<AD1CosmeticWeapon>(AD1CosmeticWeapon::StaticClass(), FTransform::Identity, Character);
									
									FTransform ComposedTransform = UKismetMathLibrary::ComposeTransforms(WeaponCDO->WeaponMeshComponent->GetRelativeTransform(), AttachInfo.AttachTransform);
									SpawnedCosmeticWeapon->SetActorRelativeTransform(ComposedTransform);
									SpawnedCosmeticWeapon->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, AttachInfo.AttachSocket);
									
									SpawnedCosmeticWeapon->WeaponMeshComponent->SetSkeletalMesh(WeaponCDO->WeaponMeshComponent->GetSkeletalMeshAsset());
									SpawnedCosmeticWeapon->FinishSpawning(FTransform::Identity, true);

									PocketStage->RefreshAlphaMaskActors();
									
									if (USkeletalMeshComponent* MeshComponent = Character->GetMesh())
									{
										UAnimSequence* PocketWorldAnim = ULyraAssetManager::GetAssetByPath<UAnimSequence>(WeaponFragment->PocketWorldAnim);
										MeshComponent->PlayAnimation(PocketWorldAnim, true);
									}
								}
							})
						);
					}
				}
			}
		}
		else if (EquippableFragment->EquipmentType == EEquipmentType::Armor)
		{
			// Refresh Real Armor Mesh
			const UD1ItemFragment_Equippable_Armor* ArmorFragment = ItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable_Armor>();
			check(ArmorFragment);

			UD1CosmeticManagerComponent* CharacterCosmetics = Character->FindComponentByClass<UD1CosmeticManagerComponent>();
			check(CharacterCosmetics);
		
			CharacterCosmetics->SetArmorMesh(ArmorFragment->ArmorType, ArmorFragment->ArmorMesh);

			// Refresh Pocket Armor Mesh
			if (Character->IsLocallyControlled())
			{
				if (UD1PocketWorldSubsystem* PocketWorldSubsystem = EquipManager->GetWorld()->GetSubsystem<UD1PocketWorldSubsystem>())
				{
					if (APlayerController* PC = Character->GetLyraPlayerController())
					{
						PocketWorldSubsystem->RegisterAndCallForGetPocketStage(PC->GetLocalPlayer(),
							FGetPocketStageDelegate::CreateLambda([ArmorFragment](AD1PocketStage* PocketStage)
							{
								if (IsValid(PocketStage))
								{
									if (UD1CosmeticManagerComponent* CosmeticManager = PocketStage->GetCosmeticManager())
									{
										CosmeticManager->SetArmorMesh(ArmorFragment->ArmorType, ArmorFragment->ArmorMesh);
									}
								}
							})
						);
					}
				}
			}
		}
	}

	if (EquippableFragment->EquipmentType == EEquipmentType::Weapon)
	{
		const UD1ItemFragment_Equippable_Weapon* WeaponFragment = ItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable_Weapon>();
		if (USkeletalMeshComponent* MeshComponent = Character->GetMesh())
		{
			if (WeaponFragment->AnimInstanceClass)
			{
				MeshComponent->LinkAnimClassLayers(WeaponFragment->AnimInstanceClass);
			}

			UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
			if (ASC && ASC->HasMatchingGameplayTag(D1GameplayTags::Status_Interact) == false)
			{
				UAnimMontage* EquipMontage = ULyraAssetManager::GetAssetByPath<UAnimMontage>(WeaponFragment->EquipMontage);
				if (UAnimInstance* AnimInstance = MeshComponent->GetAnimInstance())
				{
					if (AnimInstance->GetCurrentActiveMontage() != EquipMontage)
					{
						Character->PlayAnimMontage(EquipMontage);
					}
				}
			}
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
		
		// Despawn Real Weapon
		if (UD1EquipmentManagerComponent::IsWeaponSlot(EquipmentSlotType))
		{
			if (IsValid(SpawnedWeaponActor))
			{
				SpawnedWeaponActor->Destroy();
			}
		}
	}
	else
	{
		ALyraCharacter* Character = EquipManager->GetCharacter();
		check(Character);
		
		if (UD1EquipmentManagerComponent::IsWeaponSlot(EquipmentSlotType))
		{
			// Despawn Pocket Weapon
			if (Character->IsLocallyControlled())
			{
				if (UD1PocketWorldSubsystem* PocketWorldSubsystem = EquipManager->GetWorld()->GetSubsystem<UD1PocketWorldSubsystem>())
				{
					if (APlayerController* PC = Character->GetLyraPlayerController())
					{
						PocketWorldSubsystem->RegisterAndCallForGetPocketStage(PC->GetLocalPlayer(),
							FGetPocketStageDelegate::CreateLambda([this](AD1PocketStage* PocketStage)
							{
								if (IsValid(PocketStage))
								{
									if (IsValid(SpawnedCosmeticWeapon))
									{
										SpawnedCosmeticWeapon->Destroy();
									}
								}
							})
						);
					}
				}
			}
		}
		else if (UD1EquipmentManagerComponent::IsArmorSlot(EquipmentSlotType))
		{
			// Refresh Real Armor Mesh
			EArmorType ArmorType = EquipManager->ConvertToArmorType(EquipmentSlotType);
		
			UD1CosmeticManagerComponent* CharacterCosmetics = Character->FindComponentByClass<UD1CosmeticManagerComponent>();
			check(CharacterCosmetics);

			CharacterCosmetics->SetArmorMesh(ArmorType, nullptr);

			// Refresh Pocket Armor Mesh
			if (Character->IsLocallyControlled())
			{
				if (UD1PocketWorldSubsystem* PocketWorldSubsystem = EquipManager->GetWorld()->GetSubsystem<UD1PocketWorldSubsystem>())
				{
					if (APlayerController* PC = Character->GetLyraPlayerController())
					{
						PocketWorldSubsystem->RegisterAndCallForGetPocketStage(PC->GetLocalPlayer(),
							FGetPocketStageDelegate::CreateLambda([ArmorType](AD1PocketStage* PocketStage)
							{
								if (IsValid(PocketStage))
								{
									if (UD1CosmeticManagerComponent* CosmeticManager = PocketStage->GetCosmeticManager())
									{
										CosmeticManager->SetArmorMesh(ArmorType, nullptr);
									}
								}
							})
						);
					}
				}
			}
		}
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

void UD1EquipManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	if (IsUsingRegisteredSubObjectList())
	{
		for (const FD1EquipEntry& Entry : EquipList.Entries)
		{
			UD1ItemInstance* ItemInstance = Entry.ItemInstance;
			if (IsValid(ItemInstance))
			{
				AddReplicatedSubObject(ItemInstance);
			}
		}
	}
}

void UD1EquipManagerComponent::Equip(EEquipmentSlotType EquipmentSlotType, UD1ItemInstance* ItemInstance)
{
	check(GetOwner()->HasAuthority());

	if (EquipmentSlotType == EEquipmentSlotType::Count || ItemInstance == nullptr)
		return;
	
	EquipList.Equip(EquipmentSlotType, ItemInstance);
	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && ItemInstance)
	{
		AddReplicatedSubObject(ItemInstance);
	}
}

void UD1EquipManagerComponent::Unequip(EEquipmentSlotType EquipmentSlotType)
{
	check(GetOwner()->HasAuthority())

	if (EquipmentSlotType == EEquipmentSlotType::Count)
		return;
	
	TArray<FD1EquipEntry>& Entries = EquipList.Entries;
	FD1EquipEntry& Entry = Entries[(int32)EquipmentSlotType];
	UD1ItemInstance* RemovedItemInstance = Entry.GetItemInstance();
	
	EquipList.Unequip(EquipmentSlotType);
	if (IsUsingRegisteredSubObjectList() && RemovedItemInstance)
	{
		RemoveReplicatedSubObject(RemovedItemInstance);
	}
}

void UD1EquipManagerComponent::EquipWeaponInSlot()
{
	check(GetOwner()->HasAuthority());

	if (CurrentWeaponEquipState == EWeaponEquipState::Count)
		return;
	
	UD1EquipmentManagerComponent* EquipmentManager = GetEquipmentManagerComponent();
	check(EquipmentManager);
	
	for (EEquipmentSlotType EquipmentSlotType : Item::EquipmentSlotsByWeaponState[(int32)CurrentWeaponEquipState])
	{
		Equip(EquipmentSlotType, EquipmentManager->GetItemInstance(EquipmentSlotType));
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

	if (CurrentWeaponEquipState == EWeaponEquipState::Unarmed && NewWeaponEquipState == EWeaponEquipState::Unarmed)
		return false;

	if (CurrentWeaponEquipState == NewWeaponEquipState)
		return true;
	
	UD1EquipmentManagerComponent* EquipmentManager = GetEquipmentManagerComponent();
	check(EquipmentManager);
	
	return (EquipmentManager->IsAllEmpty(NewWeaponEquipState) == false);
}

AD1WeaponBase* UD1EquipManagerComponent::GetEquippedWeaponActor(EWeaponHandType WeaponHandType) const
{
	if (WeaponHandType == EWeaponHandType::Count)
		return nullptr;

	const TArray<FD1EquipEntry>& Entries = EquipList.Entries;
	int32 EntryIndex = (int32)ConvertToEquipmentSlotType(WeaponHandType, CurrentWeaponEquipState);
	return Entries.IsValidIndex(EntryIndex) ? Entries[EntryIndex].SpawnedWeaponActor : nullptr;
}

UD1ItemInstance* UD1EquipManagerComponent::GetEquippedWeaponItemInstance(EWeaponHandType WeaponHandType) const
{
	if (WeaponHandType == EWeaponHandType::Count)
		return nullptr;

	const TArray<FD1EquipEntry>& Entries = EquipList.Entries;
	int32 EntryIndex = (int32)ConvertToEquipmentSlotType(WeaponHandType, CurrentWeaponEquipState);
	return Entries.IsValidIndex(EntryIndex) ? Entries[EntryIndex].GetItemInstance() : nullptr;
}

AD1WeaponBase* UD1EquipManagerComponent::GetFirstEquippedWeapon() const
{
	const TArray<FD1EquipEntry>& Entries = EquipList.Entries;
	
	AD1WeaponBase* WeaponActor = nullptr;
	for (int i = 0; i < (int32)EWeaponHandType::Count; i++)
	{
		int32 EntryIndex = (int32)ConvertToEquipmentSlotType((EWeaponHandType)i, CurrentWeaponEquipState);
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
	BroadcastChangedMessage(CurrentWeaponEquipState);
}

void UD1EquipManagerComponent::BroadcastChangedMessage(EWeaponEquipState NewWeaponEquipState)
{
	// FD1WeaponEquipStateChangedMessage Message;
	// Message.WeaponEquipState = NewWeaponEquipState;
	//
	// UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());
	// MessageSubsystem.BroadcastMessage(D1GameplayTags::Message_Item_WeaponEquipStateChanged, Message);
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

EEquipmentSlotType UD1EquipManagerComponent::ConvertToEquipmentSlotType(EWeaponHandType WeaponHandType, EWeaponEquipState WeaponEquipState)
{
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;

	if (WeaponEquipState == EWeaponEquipState::Unarmed)
	{
		switch (WeaponHandType)
		{
		case EWeaponHandType::LeftHand:  EquipmentSlotType = EEquipmentSlotType::Unarmed_LeftHand;  break;
		case EWeaponHandType::RightHand: EquipmentSlotType = EEquipmentSlotType::Unarmed_RightHand; break;
		}
	}
	else if (WeaponEquipState == EWeaponEquipState::Primary)
	{
		switch (WeaponHandType)
		{
		case EWeaponHandType::LeftHand:  EquipmentSlotType = EEquipmentSlotType::Primary_LeftHand;  break;
		case EWeaponHandType::RightHand: EquipmentSlotType = EEquipmentSlotType::Primary_RightHand; break;
		case EWeaponHandType::TwoHand:   EquipmentSlotType = EEquipmentSlotType::Primary_TwoHand;   break;
		}
	}
	else if (WeaponEquipState == EWeaponEquipState::Secondary)
	{
		switch (WeaponHandType)
		{
		case EWeaponHandType::LeftHand:  EquipmentSlotType = EEquipmentSlotType::Secondary_LeftHand;  break;
		case EWeaponHandType::RightHand: EquipmentSlotType = EEquipmentSlotType::Secondary_RightHand; break;
		case EWeaponHandType::TwoHand:   EquipmentSlotType = EEquipmentSlotType::Secondary_TwoHand;   break;
		}
	}
	else if (WeaponEquipState == EWeaponEquipState::Tertiary)
	{
		switch (WeaponHandType)
		{
		case EWeaponHandType::LeftHand:  EquipmentSlotType = EEquipmentSlotType::Tertiary_LeftHand;  break;
		case EWeaponHandType::RightHand: EquipmentSlotType = EEquipmentSlotType::Tertiary_RightHand; break;
		case EWeaponHandType::TwoHand:   EquipmentSlotType = EEquipmentSlotType::Tertiary_TwoHand;   break;
		}
	}
	else if (WeaponEquipState == EWeaponEquipState::Quaternary)
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

EEquipmentSlotType UD1EquipManagerComponent::ConvertToEquipmentSlotType(EWeaponHandType WeaponHandType, EWeaponSlotType WeaponSlotType)
{
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;

	if (WeaponSlotType == EWeaponSlotType::Primary)
	{
		switch (WeaponHandType)
		{
		case EWeaponHandType::LeftHand:  EquipmentSlotType = EEquipmentSlotType::Primary_LeftHand;  break;
		case EWeaponHandType::RightHand: EquipmentSlotType = EEquipmentSlotType::Primary_RightHand; break;
		case EWeaponHandType::TwoHand:   EquipmentSlotType = EEquipmentSlotType::Primary_TwoHand;   break;
		}
	}
	else if (WeaponSlotType == EWeaponSlotType::Secondary)
	{
		switch (WeaponHandType)
		{
		case EWeaponHandType::LeftHand:  EquipmentSlotType = EEquipmentSlotType::Secondary_LeftHand;  break;
		case EWeaponHandType::RightHand: EquipmentSlotType = EEquipmentSlotType::Secondary_RightHand; break;
		case EWeaponHandType::TwoHand:   EquipmentSlotType = EEquipmentSlotType::Secondary_TwoHand;   break;
		}
	}
	else if (WeaponSlotType == EWeaponSlotType::Tertiary)
	{
		switch (WeaponHandType)
		{
		case EWeaponHandType::LeftHand:  EquipmentSlotType = EEquipmentSlotType::Tertiary_LeftHand;  break;
		case EWeaponHandType::RightHand: EquipmentSlotType = EEquipmentSlotType::Tertiary_RightHand; break;
		case EWeaponHandType::TwoHand:   EquipmentSlotType = EEquipmentSlotType::Tertiary_TwoHand;   break;
		}
	}
	else if (WeaponSlotType == EWeaponSlotType::Quaternary)
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

EEquipmentSlotType UD1EquipManagerComponent::ConvertToEquipmentSlotType(EArmorType ArmorType)
{
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;

	switch (ArmorType)
	{
	case EArmorType::Helmet: EquipmentSlotType = EEquipmentSlotType::Helmet; break;
	case EArmorType::Chest:  EquipmentSlotType = EEquipmentSlotType::Chest;  break;
	case EArmorType::Legs:   EquipmentSlotType = EEquipmentSlotType::Legs;   break;
	case EArmorType::Hands:  EquipmentSlotType = EEquipmentSlotType::Hands;  break;
	case EArmorType::Foot:   EquipmentSlotType = EEquipmentSlotType::Foot;   break;
	}

	return EquipmentSlotType;
}

EWeaponHandType UD1EquipManagerComponent::ConvertToWeaponHandType(EEquipmentSlotType EquipmentSlotType)
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

EArmorType UD1EquipManagerComponent::ConvertToArmorType(EEquipmentSlotType EquipmentSlotType)
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
