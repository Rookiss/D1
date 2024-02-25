#include "D1EquipmentManagerComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "D1GameplayTags.h"
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
	if (ItemInstance == NewItemInstance)
		return;
	
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
		Equip();
	}
}

void FD1EquipmentEntry::Equip()
{
	if (ItemInstance == nullptr)
		return;

	const UD1ItemFragment_Equippable* Equippable = ItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable>();
	check(Equippable);
	
	if (Equippable->EquipmentType == EEquipmentType::Weapon && EquipmentManager->IsSameWeaponEquipState(EquipmentSlotType, EquipmentManager->GetCurrentWeaponEquipState()) == false)
		return;
	
	UD1AbilitySystemComponent* ASC = Cast<UD1AbilitySystemComponent>(EquipmentManager->GetAbilitySystemComponent());
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
		APlayerController* OwningController = Cast<APlayerController>(EquipmentManager->GetOwner());
		check(OwningController);

		APawn* OwningPawn = OwningController->GetPawn();
		check(OwningPawn);
		
		const UD1ItemFragment_Equippable_Weapon* Weapon = ItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable_Weapon>();
		const FD1WeaponAttachInfo& AttachInfo = Weapon->WeaponAttachInfo;
		if (AttachInfo.SpawnWeaponClass)
		{
			USceneComponent* AttachTarget = OwningPawn->GetRootComponent();
			if (ACharacter* OwningCharacter = Cast<ACharacter>(OwningPawn))
			{
				AttachTarget = OwningCharacter->GetMesh();
			}
		
			UWorld* World = EquipmentManager->GetWorld();
			AD1WeaponBase* NewActor = World->SpawnActorDeferred<AD1WeaponBase>(AttachInfo.SpawnWeaponClass, FTransform::Identity, OwningPawn);
			NewActor->FinishSpawning(FTransform::Identity, true);
			NewActor->SetActorRelativeTransform(AttachInfo.AttachTransform);
			NewActor->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform, AttachInfo.AttachSocket);
		
			SpawnedWeaponActor = NewActor;
		}
	}
	else if (Equippable->EquipmentType == EEquipmentType::Armor)
	{
		// Apply New Visual
		APlayerController* OwningController = Cast<APlayerController>(EquipmentManager->GetOwner());
		check(OwningController);

		AD1Player* OwningPlayer = Cast<AD1Player>(OwningController->GetPawn());
		check(OwningPlayer);
		
		const UD1ItemFragment_Equippable_Armor* Armor = ItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable_Armor>();
		OwningPlayer->Multicast_SetArmorMesh(Armor->ArmorType, Armor->ArmorMesh.ToSoftObjectPath());
	}
}

void FD1EquipmentEntry::Unequip()
{
	if (ItemInstance == nullptr)
		return;

	UD1AbilitySystemComponent* ASC = Cast<UD1AbilitySystemComponent>(EquipmentManager->GetAbilitySystemComponent());
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
	const UD1ItemFragment_Equippable* Equippable = ItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable>();
	if (Equippable->EquipmentType == EEquipmentType::Weapon)
	{
		if (IsValid(SpawnedWeaponActor))
		{
			SpawnedWeaponActor->Destroy();
			
			if (EquipmentManager->IsAllEmpty(EquipmentManager->GetCurrentWeaponEquipState()))
			{
				FGameplayEventData Payload;
				Payload.EventMagnitude = (int32)EWeaponEquipState::Unarmed;
				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(EquipmentManager->GetOwner(), D1GameplayTags::Event_EquipWeapon, Payload);
			}
		}
	}
	else if (Equippable->EquipmentType == EEquipmentType::Armor)
	{
		APlayerController* OwningController = Cast<APlayerController>(EquipmentManager->GetOwner());
		check(OwningController);

		AD1Player* OwningPlayer = Cast<AD1Player>(OwningController->GetPawn());
		check(OwningPlayer);
		
		const UD1ItemFragment_Equippable_Armor* Armor = ItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable_Armor>();
		OwningPlayer->Multicast_SetArmorMesh(Armor->ArmorType, FSoftObjectPath());
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
		Entry.EquipmentManager = EquipmentManager;
		Entry.EquipmentSlotType = (EEquipmentSlotType)Index;
		
		UD1ItemInstance* ItemInstance = Entry.GetItemInstance();
		if (ItemInstance == nullptr)
			continue;
		
		if (EquipmentManager->OnEquipmentEntryChanged.IsBound())
		{
			EquipmentManager->OnEquipmentEntryChanged.Broadcast((EEquipmentSlotType)Index, ItemInstance);
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

void FD1EquipmentList::InitOnServer()
{
	Entries.SetNum((int32)EEquipmentSlotType::Count);

	for (int32 i = 0; i < Entries.Num(); i++)
	{
		FD1EquipmentEntry& Entry = Entries[i];
		Entry.EquipmentManager = EquipmentManager;
		Entry.EquipmentSlotType = (EEquipmentSlotType)i;
		MarkItemDirty(Entry);
	}
}

void FD1EquipmentList::SetEntry(EEquipmentSlotType EquipmentSlotType, UD1ItemInstance* ItemInstance)
{
	FD1EquipmentEntry& Entry = Entries[(int32)EquipmentSlotType];
	Entry.Init(ItemInstance);
	MarkItemDirty(Entry);
}

UD1ItemInstance* FD1EquipmentList::ResetEntry(EEquipmentSlotType EquipmentSlotType)
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
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		for (FD1EquipmentEntry& Entry : EquipmentList.Entries)
		{
			Entry.Init(nullptr);
		}
	}
	
	Super::UninitializeComponent();
}

void UD1EquipmentManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EquipmentList);
	DOREPLIFETIME(ThisClass, CurrentWeaponEquipState);
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

void UD1EquipmentManagerComponent::Server_RequestSetEntry_FromInventory_Implementation(UD1InventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, EEquipmentSlotType ToEquipmentSlotType)
{
	check(GetOwner()->HasAuthority());
	
	if (CanSetEntry_FromInventory(OtherComponent, FromItemSlotPos, ToEquipmentSlotType))
	{
		UD1ItemInstance* RemovedItemInstance = OtherComponent->InventoryList.RemoveItem_Unsafe(FromItemSlotPos, 1);
		EquipmentList.SetEntry(ToEquipmentSlotType, RemovedItemInstance);
	}
}

bool UD1EquipmentManagerComponent::CanSetEntry_FromInventory(UD1InventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, EEquipmentSlotType ToEquipmentSlotType) const
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

	return CanSetEntry(FromItemInstance, ToEquipmentSlotType);
}

void UD1EquipmentManagerComponent::Server_RequestSetEntry_FromEquipment_Implementation(UD1EquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, EEquipmentSlotType ToEquipmentSlotType)
{
	check(GetOwner()->HasAuthority());

	if (this == OtherComponent && FromEquipmentSlotType == ToEquipmentSlotType)
		return;
	
	if (CanSetEntry_FromEquipment(OtherComponent, FromEquipmentSlotType, ToEquipmentSlotType))
	{
		UD1ItemInstance* RemovedItemInstance = OtherComponent->EquipmentList.ResetEntry(FromEquipmentSlotType);
		EquipmentList.SetEntry(ToEquipmentSlotType, RemovedItemInstance);
	}
}

bool UD1EquipmentManagerComponent::CanSetEntry_FromEquipment(UD1EquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, EEquipmentSlotType ToEquipmentSlotType) const
{
	if (OtherComponent == nullptr)
		return false;
	
	if (FromEquipmentSlotType == EEquipmentSlotType::Count || ToEquipmentSlotType == EEquipmentSlotType::Count)
		return false;
	
	const TArray<FD1EquipmentEntry>& FromEntries = OtherComponent->GetAllEntries();
	const FD1EquipmentEntry& FromEntry = FromEntries[(int32)FromEquipmentSlotType];
	UD1ItemInstance* FromItemInstance = FromEntry.GetItemInstance();

	return CanSetEntry(FromItemInstance, ToEquipmentSlotType);
}

bool UD1EquipmentManagerComponent::CanSetEntry(UD1ItemInstance* FromItemInstance, EEquipmentSlotType ToEquipmentSlotType) const
{
	if (ToEquipmentSlotType == EEquipmentSlotType::Count || FromItemInstance == nullptr)
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

void UD1EquipmentManagerComponent::EquipWeaponInSlot()
{
	check(GetOwner()->HasAuthority());
	
	for (EEquipmentSlotType SlotType : Item::SlotsByWeaponEquipState[(int32)CurrentWeaponEquipState])
	{
		FD1EquipmentEntry& Entry = EquipmentList.Entries[(int32)SlotType];
		Entry.Equip();
	}
}

void UD1EquipmentManagerComponent::UnequipWeaponInSlot()
{
	check(GetOwner()->HasAuthority());

	for (EEquipmentSlotType SlotType : Item::SlotsByWeaponEquipState[(int32)CurrentWeaponEquipState])
	{
		FD1EquipmentEntry& Entry = EquipmentList.Entries[(int32)SlotType];
		Entry.Unequip();
	}
}

void UD1EquipmentManagerComponent::Server_RequestChangeWeaponEquipState_Implementation(EWeaponEquipState NewWeaponEquipState)
{
	if (CanChangeWeaponEquipState(NewWeaponEquipState))
	{
		UnequipWeaponInSlot();
		CurrentWeaponEquipState = NewWeaponEquipState;
		EquipWeaponInSlot();
	}
}

bool UD1EquipmentManagerComponent::CanChangeWeaponEquipState(EWeaponEquipState NewWeaponEquipState) const
{
	if (NewWeaponEquipState == EWeaponEquipState::Count || CurrentWeaponEquipState == NewWeaponEquipState)
		return false;

	return (IsAllEmpty(NewWeaponEquipState) == false);
}

bool UD1EquipmentManagerComponent::IsSameWeaponEquipState(EEquipmentSlotType EquipmentSlotType, EWeaponEquipState WeaponEquipState) const
{
	return ((EquipmentSlotType == EEquipmentSlotType::Primary_LeftHand   || EquipmentSlotType == EEquipmentSlotType::Primary_RightHand   || EquipmentSlotType == EEquipmentSlotType::Primary_TwoHand)   && WeaponEquipState == EWeaponEquipState::Primary ||
		    (EquipmentSlotType == EEquipmentSlotType::Secondary_LeftHand || EquipmentSlotType == EEquipmentSlotType::Secondary_RightHand || EquipmentSlotType == EEquipmentSlotType::Secondary_TwoHand) && WeaponEquipState == EWeaponEquipState::Secondary);
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

bool UD1EquipmentManagerComponent::IsPrimaryWeaponSlot(EEquipmentSlotType EquipmentSlotType) const
{
	return (EquipmentSlotType == EEquipmentSlotType::Primary_LeftHand || EquipmentSlotType == EEquipmentSlotType::Primary_RightHand || EquipmentSlotType == EEquipmentSlotType::Primary_TwoHand);
}

bool UD1EquipmentManagerComponent::IsSecondaryWeaponSlot(EEquipmentSlotType EquipmentSlotType) const
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
	for (EEquipmentSlotType SlotType : Item::SlotsByWeaponEquipState[(int32)WeaponEquipState])
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

EWeaponEquipState UD1EquipmentManagerComponent::GetBackwardWeaponEquipState() const
{
	int32 WeaponEquipStateCount = (int32)EWeaponEquipState::Count;
	return (EWeaponEquipState)(((int32)CurrentWeaponEquipState + WeaponEquipStateCount - 1) % WeaponEquipStateCount);
}

EWeaponEquipState UD1EquipmentManagerComponent::GetForwardWeaponEquipState() const
{
	int32 WeaponEquipStateCount = (int32)EWeaponEquipState::Count;
	return (EWeaponEquipState)(((int32)CurrentWeaponEquipState + 1) % WeaponEquipStateCount);
}

const TArray<FD1EquipmentEntry>& UD1EquipmentManagerComponent::GetAllEntries() const
{
	return EquipmentList.GetAllEntries();
}

UD1AbilitySystemComponent* UD1EquipmentManagerComponent::GetAbilitySystemComponent() const
{
	return Cast<UD1AbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()));
}
