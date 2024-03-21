#include "D1EquipManagerComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "D1EquipmentManagerComponent.h"
#include "AbilitySystem/D1AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/D1MonsterSet.h"
#include "AbilitySystem/Attributes/D1PlayerSet.h"
#include "Character/D1Player.h"
#include "Engine/ActorChannel.h"
#include "Item/D1ItemInstance.h"
#include "Item/Fragments/D1ItemFragment_Equippable.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Armor.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Weapon.h"
#include "Net/UnrealNetwork.h"
#include "Player/D1PlayerController.h"
#include "Weapon/D1WeaponBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1EquipManagerComponent)

void FD1EquipEntry::Init(UD1ItemInstance* NewItemInstance)
{
	if (ItemInstance == NewItemInstance)
		return;
	
	if (ItemInstance)
	{
		Unequip();
	}

	ItemInstance = NewItemInstance;

	if (ItemInstance)
	{
		Equip();
	}
}

void FD1EquipEntry::Equip()
{
	if (ItemInstance == nullptr)
		return;
	
	const UD1ItemFragment_Equippable* Equippable = ItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable>();
	check(Equippable);

	if (EquipManager->GetOwner()->HasAuthority())
	{
		UD1AbilitySystemComponent* ASC = Cast<UD1AbilitySystemComponent>(EquipManager->GetAbilitySystemComponent());
		check(ASC);
		
		// Remove Prev Ability
		GrantedHandles.TakeFromAbilitySystem(ASC);
	
		// Add New Ability
		if (const UD1AbilitySystemData* AbilitySystemData = Equippable->AbilitySystemDataToGrant)
		{
			AbilitySystemData->GiveToAbilitySystem(ASC, &GrantedHandles, ItemInstance);
		}

		// Remove Prev Stat
		for (const FActiveGameplayEffectHandle& Handle : StatHandles)
		{
			ASC->RemoveActiveGameplayEffect(Handle);
		}
		StatHandles.Reset();
	
		// Add New Stat
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
			// Remove Prev Visual
			if (IsValid(SpawnedWeaponActor))
			{
				SpawnedWeaponActor->Destroy();
			}

			// Add New Visual
			AD1Player* Player = EquipManager->GetPlayerCharacter();
			check(Player);
		
			const UD1ItemFragment_Equippable_Weapon* Weapon = ItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable_Weapon>();
			const FD1WeaponAttachInfo& AttachInfo = Weapon->WeaponAttachInfo;
			if (AttachInfo.SpawnWeaponClass)
			{
				UWorld* World = EquipManager->GetWorld();
				AD1WeaponBase* NewActor = World->SpawnActorDeferred<AD1WeaponBase>(AttachInfo.SpawnWeaponClass, FTransform::Identity, Player);
				NewActor->FinishSpawning(FTransform::Identity, true);
				NewActor->SetActorRelativeTransform(AttachInfo.AttachTransform);
				NewActor->AttachToComponent(Player->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, AttachInfo.AttachSocket);
				NewActor->ItemID = ItemInstance->GetItemID();
				NewActor->EquipmentSlotType = EquipmentSlotType;
				
				SpawnedWeaponActor = NewActor;
			}
		}
	}

	AD1Player* Player = EquipManager->GetPlayerCharacter();
	check(Player);

	if (Equippable->EquipmentType == EEquipmentType::Armor)
	{
		const UD1ItemFragment_Equippable_Armor* Armor = ItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable_Armor>();
		FSoftObjectPath ArmorMeshPath = Armor->ArmorMesh.ToSoftObjectPath();

		UD1AssetManager::GetAssetByPath(ArmorMeshPath, FAsyncLoadCompletedDelegate::CreateLambda(
		[Player, ArmorType = Armor->ArmorType](const FName& AssetName, UObject* LoadedAsset)
		{
			if (USkeletalMeshComponent* ArmorMeshComponent = Player->ArmorMeshComponents[(int32)ArmorType])
			{
				ArmorMeshComponent->SetSkeletalMesh(Cast<USkeletalMesh>(LoadedAsset));
			}
		}));
	}
}

void FD1EquipEntry::Unequip()
{
	if (ItemInstance == nullptr)
		return;

	const UD1ItemFragment_Equippable* Equippable = ItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable>();
	check(Equippable);
	
	if (EquipManager->GetOwner()->HasAuthority())
	{
		UD1AbilitySystemComponent* ASC = Cast<UD1AbilitySystemComponent>(EquipManager->GetAbilitySystemComponent());
		check(ASC);
	
		// Remove Ability
		GrantedHandles.TakeFromAbilitySystem(ASC);

		// Remove Stat
		for (const FActiveGameplayEffectHandle& Handle : StatHandles)
		{
			ASC->RemoveActiveGameplayEffect(Handle);
		}
		StatHandles.Reset();

		// Remove Visual
		if (Equippable->EquipmentType == EEquipmentType::Weapon)
		{
			if (IsValid(SpawnedWeaponActor))
			{
				SpawnedWeaponActor->Destroy();
			}
		}
	}
	
	if (Equippable->EquipmentType == EEquipmentType::Armor)
	{
		AD1Player* Player = EquipManager->GetPlayerCharacter();
		check(Player);
		
		const UD1ItemFragment_Equippable_Armor* Armor = ItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable_Armor>();
		if (USkeletalMeshComponent* ArmorMeshComponent = Player->ArmorMeshComponents[(int32)Armor->ArmorType])
		{
			ArmorMeshComponent->SetSkeletalMesh(Player->DefaultArmorMeshes[(int32)Armor->ArmorType]);
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
}

void UD1EquipManagerComponent::BeginPlay()
{
	Super::BeginPlay();

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
	
	EquipList.Equip(EquipmentSlotType, ItemInstance);
}

void UD1EquipManagerComponent::Unequip(EEquipmentSlotType EquipmentSlotType)
{
	check(GetOwner()->HasAuthority());

	EquipList.Unequip(EquipmentSlotType);
}

void UD1EquipManagerComponent::EquipWeaponInSlot()
{
	check(GetOwner()->HasAuthority());

	UD1EquipmentManagerComponent* EquipmentManager = GetEquipmentManagerComponent();
	check(EquipmentManager);

	const TArray<FD1EquipmentEntry>& Entries = EquipmentManager->GetAllEntries();
	for (EEquipmentSlotType SlotType : Item::EquipmentSlotsByWeaponState[(int32)CurrentWeaponEquipState])
	{
		Equip(SlotType, Entries[(int32)SlotType].GetItemInstance());
	}
}

void UD1EquipManagerComponent::UnequipWeaponInSlot()
{
	check(GetOwner()->HasAuthority());
	
	UD1EquipmentManagerComponent* EquipmentManager = GetEquipmentManagerComponent();
	check(EquipmentManager);
	
	for (EEquipmentSlotType SlotType : Item::EquipmentSlotsByWeaponState[(int32)CurrentWeaponEquipState])
	{
		Unequip(SlotType);
	}
}

void UD1EquipManagerComponent::ChangeWeaponEquipState(EWeaponEquipState NewWeaponEquipState)
{
	check(GetOwner()->HasAuthority());

	if (CanChangeWeaponEquipState(NewWeaponEquipState))
	{
		UnequipWeaponInSlot();
		CurrentWeaponEquipState = NewWeaponEquipState;
		EquipWeaponInSlot();
	}
}

bool UD1EquipManagerComponent::CanChangeWeaponEquipState(EWeaponEquipState NewWeaponEquipState) const
{
	if (NewWeaponEquipState == EWeaponEquipState::Count || CurrentWeaponEquipState == NewWeaponEquipState)
		return false;

	UD1EquipmentManagerComponent* EquipmentManager = GetEquipmentManagerComponent();
	return (EquipmentManager->IsAllEmpty(NewWeaponEquipState) == false);
}

EWeaponEquipState UD1EquipManagerComponent::GetBackwardWeaponEquipState() const
{
	int32 WeaponEquipStateCount = (int32)EWeaponEquipState::Count;
	return (EWeaponEquipState)(((int32)CurrentWeaponEquipState + WeaponEquipStateCount - 1) % WeaponEquipStateCount);
}

EWeaponEquipState UD1EquipManagerComponent::GetForwardWeaponEquipState() const
{
	int32 WeaponEquipStateCount = (int32)EWeaponEquipState::Count;
	return (EWeaponEquipState)(((int32)CurrentWeaponEquipState + 1) % WeaponEquipStateCount);
}

void UD1EquipManagerComponent::OnRep_CurrentWeaponEquipState()
{
	if (OnWeaponEquipStateChanged.IsBound())
	{
		OnWeaponEquipStateChanged.Broadcast(CurrentWeaponEquipState);
	}
}

AD1Player* UD1EquipManagerComponent::GetPlayerCharacter() const
{
	return Cast<AD1Player>(GetOwner());
}

AD1PlayerController* UD1EquipManagerComponent::GetPlayerController() const
{
	if (AD1Player* Player = GetPlayerCharacter())
	{
		return Cast<AD1PlayerController>(Player->Controller);
	}
	return nullptr;
}

TArray<FD1EquipEntry>& UD1EquipManagerComponent::GetAllEntries()
{
	return EquipList.GetAllEntries();
}

UD1AbilitySystemComponent* UD1EquipManagerComponent::GetAbilitySystemComponent() const
{
	return Cast<UD1AbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()));
}

UD1EquipmentManagerComponent* UD1EquipManagerComponent::GetEquipmentManagerComponent() const
{
	if (AD1Player* Character = GetPlayerCharacter())
	{
		return Character->EquipmentManagerComponent;
	}
	return nullptr;
}

EEquipmentSlotType UD1EquipManagerComponent::ConvertToEquipmentSlotType(EWeaponHandType WeaponHandType) const
{
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;

	if (CurrentWeaponEquipState == EWeaponEquipState::Unarmed)
	{
		EquipmentSlotType = EEquipmentSlotType::Unarmed;
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
	
	return EquipmentSlotType;
}
