#include "D1EquipManagerComponent.h"

#include "AbilitySystemGlobals.h"
#include "D1CosmeticManagerComponent.h"
#include "D1EquipmentManagerComponent.h"
#include "D1GameplayTags.h"
#include "Engine/ActorChannel.h"
#include "Item/D1ItemInstance.h"
#include "Item/Fragments/D1ItemFragment_Equipable.h"
#include "Item/Fragments/D1ItemFragment_Equipable_Armor.h"
#include "Item/Fragments/D1ItemFragment_Equipable_Weapon.h"
#include "Net/UnrealNetwork.h"
#include "Actors/D1EquipmentBase.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/D1CombatSet.h"
#include "Character/LyraCharacter.h"
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
	if (Character == nullptr)
		return;

	const UD1ItemFragment_Equipable* EquippableFragment = ItemInstance->FindFragmentByClass<UD1ItemFragment_Equipable>();
	if (EquippableFragment == nullptr)
		return;

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
		
		if (EquippableFragment->EquipmentType == EEquipmentType::Weapon || EquippableFragment->EquipmentType == EEquipmentType::Utility)
		{
			// Despawn Previous Real Weapon
			if (IsValid(SpawnedEquipmentActor))
			{
				SpawnedEquipmentActor->Destroy();
			}

			// Spawn Current Real Weapon
			const UD1ItemFragment_Equipable_Attachment* AttachmentFragment = ItemInstance->FindFragmentByClass<UD1ItemFragment_Equipable_Attachment>();
			const FD1WeaponAttachInfo& AttachInfo = AttachmentFragment->WeaponAttachInfo;
			if (AttachInfo.SpawnWeaponClass)
			{
				UWorld* World = EquipManager->GetWorld();
				AD1EquipmentBase* NewWeaponActor = World->SpawnActorDeferred<AD1EquipmentBase>(AttachInfo.SpawnWeaponClass, FTransform::Identity, Character);
				NewWeaponActor->Init(ItemInstance->GetItemTemplateID(), EquipmentSlotType);
				NewWeaponActor->SetActorRelativeTransform(AttachInfo.AttachTransform);
				NewWeaponActor->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, AttachInfo.AttachSocket);
				NewWeaponActor->SetActorHiddenInGame(EquipManager->ShouldHiddenEquipments());
				NewWeaponActor->FinishSpawning(FTransform::Identity, true);
			}
		}
	}
	else
	{
		if (EquippableFragment->EquipmentType == EEquipmentType::Weapon || EquippableFragment->EquipmentType == EEquipmentType::Utility)
		{
			if (Character->IsLocallyControlled())
			{
				// Despawn Previous Pocket Weapon
				if (IsValid(SpawnedPocketWorldActor))
				{
					SpawnedPocketWorldActor->Destroy();
				}

				// Spawn Current Pocket Weapon
				const UD1ItemFragment_Equipable_Attachment* AttachmentFragment = ItemInstance->FindFragmentByClass<UD1ItemFragment_Equipable_Attachment>();
				if (UD1PocketWorldSubsystem* PocketWorldSubsystem = EquipManager->GetWorld()->GetSubsystem<UD1PocketWorldSubsystem>())
				{
					if (APlayerController* PC = Character->GetLyraPlayerController())
					{
						PocketWorldSubsystem->RegisterAndCallForGetPocketStage(PC->GetLocalPlayer(),
							FGetPocketStageDelegate::CreateLambda([this, AttachmentFragment](AD1PocketStage* PocketStage)
							{
								if (IsValid(PocketStage))
								{
									ACharacter* Character = PocketStage->GetCharacter();
									const FD1WeaponAttachInfo& AttachInfo = AttachmentFragment->WeaponAttachInfo;
									
									UWorld* World = EquipManager->GetWorld();
									SpawnedPocketWorldActor = World->SpawnActorDeferred<AD1EquipmentBase>(AttachInfo.SpawnWeaponClass, FTransform::Identity, Character);
									SpawnedPocketWorldActor->SetActorRelativeTransform(AttachInfo.AttachTransform);
									SpawnedPocketWorldActor->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, AttachInfo.AttachSocket);
									SpawnedPocketWorldActor->bOnlyUseForLocal = true;
									SpawnedPocketWorldActor->FinishSpawning(FTransform::Identity, true);

									PocketStage->RefreshLightingChannelToActors();
									
									UAnimMontage* PocketWorldIdleMontage = ULyraAssetManager::GetAssetByPath<UAnimMontage>(AttachmentFragment->PocketWorldIdleMontage);
									Character->PlayAnimMontage(PocketWorldIdleMontage);
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
			const UD1ItemFragment_Equipable_Armor* ArmorFragment = ItemInstance->FindFragmentByClass<UD1ItemFragment_Equipable_Armor>();
			if (UD1CosmeticManagerComponent* CharacterCosmetics = Character->FindComponentByClass<UD1CosmeticManagerComponent>())
			{
				CharacterCosmetics->RefreshArmorMesh(ArmorFragment->ArmorType, ArmorFragment);
			}

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
										CosmeticManager->RefreshArmorMesh(ArmorFragment->ArmorType, ArmorFragment);
									}
								}
							})
						);
					}
				}
			}
		}
	}

	if (EquippableFragment->EquipmentType == EEquipmentType::Weapon || EquippableFragment->EquipmentType == EEquipmentType::Utility)
	{
		const UD1ItemFragment_Equipable_Attachment* AttachmentFragment = ItemInstance->FindFragmentByClass<UD1ItemFragment_Equipable_Attachment>();
		if (USkeletalMeshComponent* MeshComponent = Character->GetMesh())
		{
			if (AttachmentFragment->AnimInstanceClass)
			{
				MeshComponent->LinkAnimClassLayers(AttachmentFragment->AnimInstanceClass);
			}

			UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
			if (ASC && ASC->HasMatchingGameplayTag(D1GameplayTags::Status_Interact) == false)
			{
				UAnimMontage* EquipMontage = ULyraAssetManager::GetAssetByPath<UAnimMontage>(AttachmentFragment->EquipMontage);
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
		if (UD1EquipmentManagerComponent::IsWeaponSlot(EquipmentSlotType) || UD1EquipmentManagerComponent::IsUtilitySlot(EquipmentSlotType))
		{
			if (IsValid(SpawnedEquipmentActor))
			{
				SpawnedEquipmentActor->Destroy();
			}
		}
	}
	else
	{
		if (ALyraCharacter* Character = EquipManager->GetCharacter())
		{
			if (UD1EquipmentManagerComponent::IsWeaponSlot(EquipmentSlotType) || UD1EquipmentManagerComponent::IsUtilitySlot(EquipmentSlotType))
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
										if (IsValid(SpawnedPocketWorldActor))
										{
											SpawnedPocketWorldActor->Destroy();
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
		
				if (UD1CosmeticManagerComponent* CharacterCosmetics = Character->FindComponentByClass<UD1CosmeticManagerComponent>())
				{
					CharacterCosmetics->RefreshArmorMesh(ArmorType, nullptr);
				}
				
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
											CosmeticManager->RefreshArmorMesh(ArmorType, nullptr);
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
}

bool FD1EquipList::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
{
	return FFastArraySerializer::FastArrayDeltaSerialize<FD1EquipEntry, FD1EquipList>(Entries, DeltaParams,*this);
}

void FD1EquipList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 AddedIndex : AddedIndices)
	{
		FD1EquipEntry& Entry = Entries[AddedIndex];
		Entry.EquipManager = EquipManager;
		Entry.EquipmentSlotType = (EEquipmentSlotType)AddedIndex;
		
		if (Entry.GetItemInstance())
		{
			Entry.Equip();
		}
	}
}

void FD1EquipList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 ChangedIndex : ChangedIndices)
	{
		FD1EquipEntry& Entry = Entries[ChangedIndex];
		Entry.GetItemInstance() ? Entry.Equip() : Entry.Unequip();
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
	DOREPLIFETIME(ThisClass, CurrentEquipState);
	DOREPLIFETIME(ThisClass, bShouldHiddenEquipments);
}

bool UD1EquipManagerComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	
	for (FD1EquipEntry& Entry : EquipList.Entries)
	{
		UD1ItemInstance* ItemInstance = Entry.GetItemInstance();
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
			UD1ItemInstance* ItemInstance = Entry.GetItemInstance();
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

void UD1EquipManagerComponent::EquipCurrentSlots()
{
	check(GetOwner()->HasAuthority());

	if (CurrentEquipState == EEquipState::Count)
		return;
	
	if (UD1EquipmentManagerComponent* EquipmentManager = GetEquipmentManager())
	{
		for (EEquipmentSlotType EquipmentSlotType : UD1EquipManagerComponent::GetEquipmentSlotsByEquipState(CurrentEquipState))
		{
			Equip(EquipmentSlotType, EquipmentManager->GetItemInstance(EquipmentSlotType));
		}
	}
}

void UD1EquipManagerComponent::UnequipCurrentSlots()
{
	check(GetOwner()->HasAuthority());

	if (CurrentEquipState == EEquipState::Count)
		return;
	
	for (EEquipmentSlotType EquipmentSlotType : UD1EquipManagerComponent::GetEquipmentSlotsByEquipState(CurrentEquipState))
	{
		Unequip(EquipmentSlotType);
	}
}

void UD1EquipManagerComponent::ChangeEquipState(EEquipState NewEquipState)
{
	check(GetOwner()->HasAuthority());

	if (CanChangeEquipState(NewEquipState))
	{
		if (CurrentEquipState == NewEquipState)
		{
			NewEquipState = EEquipState::Unarmed;
		}
		
		UnequipCurrentSlots();
		CurrentEquipState = NewEquipState;
		EquipCurrentSlots();
	}
}

bool UD1EquipManagerComponent::CanChangeEquipState(EEquipState NewEquipState) const
{
	if (NewEquipState == EEquipState::Count)
		return false;

	if (CurrentEquipState == EEquipState::Unarmed && NewEquipState == EEquipState::Unarmed)
		return false;

	if (CurrentEquipState == NewEquipState)
		return true;
	
	UD1EquipmentManagerComponent* EquipmentManager = GetEquipmentManager();
	if (EquipmentManager == nullptr)
		return false;
	
	return (EquipmentManager->IsAllEmpty(NewEquipState) == false);
}

AD1EquipmentBase* UD1EquipManagerComponent::GetFirstEquippedActor() const
{
	AD1EquipmentBase* EquipmentActor = nullptr;
	const TArray<FD1EquipEntry>& Entries = EquipList.Entries;

	if (IsWeaponEquipState(CurrentEquipState))
	{
		for (int i = 0; i < (int32)EWeaponHandType::Count; i++)
		{
			const int32 EntryIndex = (int32)ConvertToEquipmentSlotType((EWeaponHandType)i, CurrentEquipState);
			if (Entries.IsValidIndex(EntryIndex) == false)
				continue;

			EquipmentActor = Entries[EntryIndex].GetEquipmentActor();
			if (EquipmentActor)
				break;
		}
	}
	else if (IsUtilityEquipState(CurrentEquipState))
	{
		const int32 EntryIndex = (int32)ConvertToEquipmentSlotType(EWeaponHandType::Count, CurrentEquipState);
		if (Entries.IsValidIndex(EntryIndex))
		{
			EquipmentActor = Entries[EntryIndex].GetEquipmentActor();
		}
	}
	
	return EquipmentActor;
}

AD1EquipmentBase* UD1EquipManagerComponent::GetEquippedActor(EWeaponHandType WeaponHandType) const
{
	if (WeaponHandType == EWeaponHandType::Count)
		return GetFirstEquippedActor();
	
	const TArray<FD1EquipEntry>& Entries = EquipList.Entries;
	const int32 EntryIndex = (int32)ConvertToEquipmentSlotType(WeaponHandType, CurrentEquipState);
	return Entries.IsValidIndex(EntryIndex) ? Entries[EntryIndex].GetEquipmentActor() : nullptr;
}

void UD1EquipManagerComponent::GetAllEquippedActors(TArray<AD1EquipmentBase*>& OutActors) const
{
	OutActors.Reset();
	
	const TArray<FD1EquipEntry>& Entries = EquipList.Entries;
	
	if (IsWeaponEquipState(CurrentEquipState))
	{
		for (int32 i = 0; i < (int32)EWeaponHandType::Count; i++)
		{
			const int32 EntryIndex = (int32)ConvertToEquipmentSlotType((EWeaponHandType)i, CurrentEquipState);
			if (Entries.IsValidIndex(EntryIndex) && Entries[EntryIndex].GetEquipmentActor())
			{
				OutActors.Add(Entries[EntryIndex].GetEquipmentActor());
			}
		}
	}
	else if (IsUtilityEquipState(CurrentEquipState))
	{
		const int32 EntryIndex = (int32)ConvertToEquipmentSlotType(EWeaponHandType::Count, CurrentEquipState);
		if (Entries.IsValidIndex(EntryIndex) && Entries[EntryIndex].GetEquipmentActor())
		{
			OutActors.Add(Entries[EntryIndex].GetEquipmentActor());
		}
	}
}

UD1ItemInstance* UD1EquipManagerComponent::GetFirstEquippedItemInstance(bool bIgnoreArmor) const
{
	const TArray<FD1EquipEntry>& Entries = EquipList.Entries;

	if (bIgnoreArmor == false)
	{
		for (int i = 0; i < (int32)EArmorType::Count; i++)
		{
			const int32 EntryIndex = (int32)ConvertToEquipmentSlotType((EArmorType)i);
			if (Entries.IsValidIndex(EntryIndex) == false)
				continue;

			if (UD1ItemInstance* ItemInstance = Entries[EntryIndex].GetItemInstance())
				return ItemInstance;
		}
	}

	UD1ItemInstance* ItemInstance = nullptr;
	
	if (IsWeaponEquipState(CurrentEquipState))
	{
		for (int i = 0; i < (int32)EWeaponHandType::Count; i++)
		{
			const int32 EntryIndex = (int32)ConvertToEquipmentSlotType((EWeaponHandType)i, CurrentEquipState);
			if (Entries.IsValidIndex(EntryIndex) == false)
				continue;

			ItemInstance = Entries[EntryIndex].GetItemInstance();
			if (ItemInstance)
				break;
		}
	}
	else if (IsUtilityEquipState(CurrentEquipState))
	{
		const int32 EntryIndex = (int32)ConvertToEquipmentSlotType(EWeaponHandType::Count, CurrentEquipState);
		if (Entries.IsValidIndex(EntryIndex))
		{
			ItemInstance = Entries[EntryIndex].GetItemInstance();
		}
	}
	
	return ItemInstance;
}

UD1ItemInstance* UD1EquipManagerComponent::GetEquippedItemInstance(EArmorType ArmorType) const
{
	const TArray<FD1EquipEntry>& Entries = EquipList.Entries;
	const int32 EntryIndex = (int32)ConvertToEquipmentSlotType(ArmorType);
	return Entries.IsValidIndex(EntryIndex) ? Entries[EntryIndex].GetItemInstance() : nullptr;
}

UD1ItemInstance* UD1EquipManagerComponent::GetEquippedItemInstance(EWeaponHandType WeaponHandType) const
{
	if (WeaponHandType == EWeaponHandType::Count)
		return GetFirstEquippedItemInstance();
	
	const TArray<FD1EquipEntry>& Entries = EquipList.Entries;
	const int32 EntryIndex = (int32)ConvertToEquipmentSlotType(WeaponHandType, CurrentEquipState);
	return Entries.IsValidIndex(EntryIndex) ? Entries[EntryIndex].GetItemInstance() : nullptr;
}

UD1ItemInstance* UD1EquipManagerComponent::GetEquippedItemInstance(EEquipmentSlotType EquipmentSlotType) const
{
	if (EquipmentSlotType == EEquipmentSlotType::Count)
		return nullptr;

	const TArray<FD1EquipEntry>& Entries = EquipList.Entries;
	const int32 EntryIndex = (int32)EquipmentSlotType;
	return Entries.IsValidIndex(EntryIndex) ? Entries[EntryIndex].GetItemInstance() : nullptr;
}

void UD1EquipManagerComponent::OnRep_CurrentEquipState(EEquipState PrevEquipState)
{
	BroadcastChangedMessage(PrevEquipState, CurrentEquipState);
}

void UD1EquipManagerComponent::BroadcastChangedMessage(EEquipState PrevEquipState, EEquipState NewEquipState)
{
	if (OnEquipStateChanged.IsBound())
	{
		OnEquipStateChanged.Broadcast(PrevEquipState, NewEquipState);
	}
}

ALyraCharacter* UD1EquipManagerComponent::GetCharacter() const
{
	return Cast<ALyraCharacter>(GetOwner());
}

ALyraPlayerController* UD1EquipManagerComponent::GetPlayerController() const
{
	if (ALyraCharacter* LyraCharacter = GetCharacter())
	{
		return LyraCharacter->GetLyraPlayerController();
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

UD1EquipmentManagerComponent* UD1EquipManagerComponent::GetEquipmentManager() const
{
	if (ALyraCharacter* LyraCharacter = GetCharacter())
	{
		return LyraCharacter->FindComponentByClass<UD1EquipmentManagerComponent>();
	}
	return nullptr;
}

EEquipmentSlotType UD1EquipManagerComponent::ConvertToEquipmentSlotType(EWeaponHandType WeaponHandType, EEquipState EquipState)
{
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;

	if (EquipState == EEquipState::Unarmed)
	{
		switch (WeaponHandType)
		{
		case EWeaponHandType::LeftHand:  EquipmentSlotType = EEquipmentSlotType::Unarmed_LeftHand;  break;
		case EWeaponHandType::RightHand: EquipmentSlotType = EEquipmentSlotType::Unarmed_RightHand; break;
		}
	}
	else if (EquipState == EEquipState::Weapon_Primary)
	{
		switch (WeaponHandType)
		{
		case EWeaponHandType::LeftHand:  EquipmentSlotType = EEquipmentSlotType::Primary_LeftHand;  break;
		case EWeaponHandType::RightHand: EquipmentSlotType = EEquipmentSlotType::Primary_RightHand; break;
		case EWeaponHandType::TwoHand:   EquipmentSlotType = EEquipmentSlotType::Primary_TwoHand;   break;
		}
	}
	else if (EquipState == EEquipState::Weapon_Secondary)
	{
		switch (WeaponHandType)
		{
		case EWeaponHandType::LeftHand:  EquipmentSlotType = EEquipmentSlotType::Secondary_LeftHand;  break;
		case EWeaponHandType::RightHand: EquipmentSlotType = EEquipmentSlotType::Secondary_RightHand; break;
		case EWeaponHandType::TwoHand:   EquipmentSlotType = EEquipmentSlotType::Secondary_TwoHand;   break;
		}
	}
	else if (EquipState == EEquipState::Utility_Primary)
	{
		EquipmentSlotType = EEquipmentSlotType::Utility_Primary;
	}
	else if (EquipState == EEquipState::Utility_Secondary)
	{
		EquipmentSlotType = EEquipmentSlotType::Utility_Secondary;
	}
	else if (EquipState == EEquipState::Utility_Tertiary)
	{
		EquipmentSlotType = EEquipmentSlotType::Utility_Tertiary;
	}
	else if (EquipState == EEquipState::Utility_Quaternary)
	{
		EquipmentSlotType = EEquipmentSlotType::Utility_Quaternary;
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

EEquipmentSlotType UD1EquipManagerComponent::ConvertToEquipmentSlotType(EUtilitySlotType UtilitySlotType)
{
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;

	switch (UtilitySlotType)
	{
	case EUtilitySlotType::Primary:		EquipmentSlotType = EEquipmentSlotType::Utility_Primary;	break;
	case EUtilitySlotType::Secondary:	EquipmentSlotType = EEquipmentSlotType::Utility_Secondary;	break;
	case EUtilitySlotType::Tertiary:	EquipmentSlotType = EEquipmentSlotType::Utility_Tertiary;	break;
	case EUtilitySlotType::Quaternary:	EquipmentSlotType = EEquipmentSlotType::Utility_Quaternary;	break;
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
		WeaponHandType = EWeaponHandType::LeftHand;
		break;
	case EEquipmentSlotType::Unarmed_RightHand:
	case EEquipmentSlotType::Primary_RightHand:
	case EEquipmentSlotType::Secondary_RightHand:
		WeaponHandType = EWeaponHandType::RightHand;
		break;
	case EEquipmentSlotType::Primary_TwoHand:
	case EEquipmentSlotType::Secondary_TwoHand:
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

EUtilitySlotType UD1EquipManagerComponent::ConvertToUtilitySlotType(EEquipmentSlotType EquipmentSlotType)
{
	EUtilitySlotType UtilitySlotType = EUtilitySlotType::Count;

	switch (EquipmentSlotType)
	{
	case EEquipmentSlotType::Utility_Primary:		UtilitySlotType = EUtilitySlotType::Primary;	break;
	case EEquipmentSlotType::Utility_Secondary:		UtilitySlotType = EUtilitySlotType::Secondary;	break;
	case EEquipmentSlotType::Utility_Tertiary:		UtilitySlotType = EUtilitySlotType::Tertiary;	break;
	case EEquipmentSlotType::Utility_Quaternary:	UtilitySlotType = EUtilitySlotType::Quaternary;	break;
	}

	return UtilitySlotType;
}

EEquipState UD1EquipManagerComponent::ConvertToEquipState(EWeaponSlotType WeaponSlotType)
{
	EEquipState EquipState = EEquipState::Count;

	switch (WeaponSlotType)
	{
	case EWeaponSlotType::Primary:		EquipState = EEquipState::Weapon_Primary;		break;
	case EWeaponSlotType::Secondary:	EquipState = EEquipState::Weapon_Secondary;		break;
	}

	return EquipState;
}

EEquipState UD1EquipManagerComponent::ConvertToEquipState(EUtilitySlotType UtilitySlotType)
{
	EEquipState EquipState = EEquipState::Count;

	switch (UtilitySlotType)
	{
	case EUtilitySlotType::Primary:		EquipState = EEquipState::Utility_Primary;		break;
	case EUtilitySlotType::Secondary:	EquipState = EEquipState::Utility_Secondary;	break;
	case EUtilitySlotType::Tertiary:	EquipState = EEquipState::Utility_Tertiary;		break;
	case EUtilitySlotType::Quaternary:	EquipState = EEquipState::Utility_Quaternary;	break;
	}

	return EquipState;
}

bool UD1EquipManagerComponent::IsWeaponEquipState(EEquipState EquipState)
{
	return (EEquipState::Unarmed <= EquipState && EquipState <= EEquipState::Weapon_Secondary);
}

bool UD1EquipManagerComponent::IsUtilityEquipState(EEquipState EquipState)
{
	return (EEquipState::Utility_Primary <= EquipState && EquipState <= EEquipState::Utility_Quaternary);
}

const TArray<EEquipmentSlotType>& UD1EquipManagerComponent::GetEquipmentSlotsByEquipState(EEquipState EquipState)
{
	static const TArray<TArray<EEquipmentSlotType>> EquipmentSlotsByEquipState = {
		{ EEquipmentSlotType::Unarmed_LeftHand,    EEquipmentSlotType::Unarmed_RightHand                                            },
		{ EEquipmentSlotType::Primary_LeftHand,    EEquipmentSlotType::Primary_RightHand,    EEquipmentSlotType::Primary_TwoHand    },
		{ EEquipmentSlotType::Secondary_LeftHand,  EEquipmentSlotType::Secondary_RightHand,  EEquipmentSlotType::Secondary_TwoHand  },
		{ EEquipmentSlotType::Utility_Primary }, { EEquipmentSlotType::Utility_Secondary }, { EEquipmentSlotType::Utility_Tertiary }, { EEquipmentSlotType::Utility_Quaternary },
	};

	if (EquipmentSlotsByEquipState.IsValidIndex((int32)EquipState))
	{
		return EquipmentSlotsByEquipState[(int32)EquipState];
	}
	else
	{
		static const TArray<EEquipmentSlotType> EmptyEquipmentSlots;
		return EmptyEquipmentSlots;
	}
}

EWeaponSlotType UD1EquipManagerComponent::ConvertToWeaponSlotType(EEquipmentSlotType EquipmentSlotType)
{
	EWeaponSlotType WeaponSlotType = EWeaponSlotType::Count;

	switch (EquipmentSlotType)
	{
	case EEquipmentSlotType::Primary_LeftHand:
	case EEquipmentSlotType::Primary_RightHand:
	case EEquipmentSlotType::Primary_TwoHand:
		WeaponSlotType = EWeaponSlotType::Primary;
		break;
	case EEquipmentSlotType::Secondary_LeftHand:
	case EEquipmentSlotType::Secondary_RightHand:
	case EEquipmentSlotType::Secondary_TwoHand:
		WeaponSlotType = EWeaponSlotType::Secondary;
		break;
	}

	return WeaponSlotType;
}

void UD1EquipManagerComponent::ChangeShouldHiddenEquipments(bool bNewShouldHiddenEquipments)
{
	bShouldHiddenEquipments = bNewShouldHiddenEquipments;

	TArray<AD1EquipmentBase*> OutEquippedActors;
	GetAllEquippedActors(OutEquippedActors);

	for (AD1EquipmentBase* WeaponActor : OutEquippedActors)
	{
		if (IsValid(WeaponActor))
		{
			WeaponActor->SetActorHiddenInGame(bShouldHiddenEquipments);
		}
	}
}
