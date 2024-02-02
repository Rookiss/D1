#include "D1EquipmentManagerComponent.h"

#include "AbilitySystemGlobals.h"
#include "D1InventoryManagerComponent.h"
#include "AbilitySystem/D1AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/D1PlayerSet.h"
#include "Character/D1Player.h"
#include "Engine/ActorChannel.h"
#include "GameFramework/Character.h"
#include "Item/Fragments/D1ItemFragment_Consumable.h"
#include "Item/Fragments/D1ItemFragment_Equippable.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Armor.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Weapon.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1EquipmentManagerComponent)

void FD1EquipmentEntry::Init(UD1ItemInstance* InItemInstance)
{
	check(InItemInstance);
	
	const UD1ItemFragment_Equippable* Equippable = InItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable>();
	check(Equippable);

	UD1AbilitySystemComponent* ASC = Cast<UD1AbilitySystemComponent>(OwnerComponent->GetAbilitySystemComponent());
	
	ItemInstance = InItemInstance;
	LatestValidItemID = ItemInstance->ItemID;
	GrantedHandles.TakeFromAbilitySystem(ASC);
	
	if (const UD1AbilitySystemData* AbilitySystemData = Equippable->AbilitySystemDataToGrant)
	{
		AbilitySystemData->GiveToAbilitySystem(ASC, &GrantedHandles, ItemInstance);
	}
	
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
	
	FActiveGameplayEffectHandle Handle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	StatHandles.Add(Handle);

	if (Equippable->EquipmentType == EEquipmentType::Weapon)
	{
		const UD1ItemFragment_Equippable_Weapon* Weapon = ItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable_Weapon>();
		const FD1WeaponAttachInfo& AttachInfo = Weapon->WeaponAttachInfo;
		if (AttachInfo.SpawnActorClass)
		{
			APawn* OwningPawn = Cast<APawn>(OwnerComponent->GetOwner());
			check(OwningPawn);
	
			USceneComponent* AttachTarget = OwningPawn->GetRootComponent();
			if (ACharacter* OwningCharacter = Cast<ACharacter>(OwningPawn))
			{
				AttachTarget = OwningCharacter->GetMesh();
			}

			UWorld* World = OwnerComponent->GetWorld();
			AActor* NewActor = World->SpawnActorDeferred<AActor>(AttachInfo.SpawnActorClass, FTransform::Identity, OwningPawn);
			NewActor->FinishSpawning(FTransform::Identity, true);
			NewActor->SetActorRelativeTransform(AttachInfo.AttachTransform);
			NewActor->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform, AttachInfo.AttachSocket);
		
			SpawnedActor.Add(NewActor);
		}
	}
	else if (Equippable->EquipmentType == EEquipmentType::Armor)
	{
		const UD1ItemFragment_Equippable_Armor* Armor = ItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable_Armor>();
		AD1Player* Player = Cast<AD1Player>(OwnerComponent->GetOwner());
		Player->Multicast_SetArmorMesh(Armor->ArmorType, Armor->ArmorMesh.ToSoftObjectPath());
	}
}

void FD1EquipmentEntry::Reset()
{
	if (ItemInstance == nullptr)
		return;
	
	GrantedHandles.TakeFromAbilitySystem(OwnerComponent->GetAbilitySystemComponent());

	UD1AbilitySystemComponent* ASC = Cast<UD1AbilitySystemComponent>(OwnerComponent->GetAbilitySystemComponent());
	for (const FActiveGameplayEffectHandle& Handle : StatHandles)
	{
		ASC->RemoveActiveGameplayEffect(Handle);
	}
	StatHandles.Reset();

	const UD1ItemFragment_Equippable* Equippable = ItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable>();
	if (Equippable->EquipmentType == EEquipmentType::Weapon)
	{
		for (AActor* Actor : SpawnedActor)
		{
			if (IsValid(Actor))
			{
				Actor->Destroy();
			}
		}
		SpawnedActor.Reset();
	}
	else if (Equippable->EquipmentType == EEquipmentType::Armor)
	{
		const UD1ItemFragment_Equippable_Armor* Armor = ItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable_Armor>();
		AD1Player* Player = Cast<AD1Player>(OwnerComponent->GetOwner());
		Player->Multicast_SetArmorMesh(Armor->ArmorType, FSoftObjectPath());
	}
	
	ItemInstance = nullptr;
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
		Entry.OwnerComponent = OwnerComponent;
		
		UD1ItemInstance* ItemInstance = Entry.GetItemInstance();
		if (ItemInstance == nullptr)
			continue;
		
		if (OwnerComponent->OnEquipmentEntryChanged.IsBound())
		{
			OwnerComponent->OnEquipmentEntryChanged.Broadcast(static_cast<EEquipmentSlotType>(Index), ItemInstance);
		}
	}
}

void FD1EquipmentList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		const FD1EquipmentEntry& Entry = Entries[Index];
		
		if (OwnerComponent->OnEquipmentEntryChanged.IsBound())
		{
			OwnerComponent->OnEquipmentEntryChanged.Broadcast(static_cast<EEquipmentSlotType>(Index), Entry.ItemInstance);
		}
	}
}

void FD1EquipmentList::InitOnServer()
{
	Entries.SetNum(OwnerComponent->GetEquipmentSlotCount());

	for (FD1EquipmentEntry& Entry : Entries)
	{
		Entry.OwnerComponent = OwnerComponent;
		MarkItemDirty(Entry);
	}
}

void FD1EquipmentList::EquipItem_Unsafe(EEquipmentSlotType EquipmentSlotType, UD1ItemInstance* ItemInstance)
{
	FD1EquipmentEntry& Entry = Entries[EquipmentSlotType];
	Entry.Init(ItemInstance);
	MarkItemDirty(Entry);
}

UD1ItemInstance* FD1EquipmentList::UnequipItem_Unsafe(EEquipmentSlotType EquipmentSlotType)
{
	FD1EquipmentEntry& Entry = Entries[EquipmentSlotType];
	UD1ItemInstance* ItemInstance = Entry.ItemInstance;
	Entry.Reset();
	MarkItemDirty(Entry);
	return ItemInstance;
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

	if (FromEquipmentSlotType != ToEquipmentSlotType)
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
		
		if (IsPrimaryWeaponSlot(ToEquipmentSlotType))
		{
			if (WeaponHandType == EWeaponHandType::LeftHand || WeaponHandType == EWeaponHandType::RightHand)
			{
				const FD1EquipmentEntry& TwoHandEntry = Entries[Weapon_Primary_TwoHand];
				return (TwoHandEntry.ItemInstance == nullptr);
			}
			else if (WeaponHandType == EWeaponHandType::TwoHand)
			{
				const FD1EquipmentEntry& LeftHandEntry = Entries[Weapon_Primary_LeftHand];
				const FD1EquipmentEntry& RightHandEntry = Entries[Weapon_Primary_RightHand];
				return (LeftHandEntry.ItemInstance == nullptr && RightHandEntry.ItemInstance == nullptr);
			}
		}
		else if (IsSecondaryWeaponSlot(ToEquipmentSlotType))
		{
			if (WeaponHandType == EWeaponHandType::LeftHand || WeaponHandType == EWeaponHandType::RightHand)
			{
				const FD1EquipmentEntry& TwoHandEntry = Entries[Weapon_Secondary_TwoHand];
				return (TwoHandEntry.ItemInstance == nullptr);
			}
			else if (WeaponHandType == EWeaponHandType::TwoHand)
			{
				const FD1EquipmentEntry& LeftHandEntry = Entries[Weapon_Secondary_LeftHand];
				const FD1EquipmentEntry& RightHandEntry = Entries[Weapon_Secondary_RightHand];
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

bool UD1EquipmentManagerComponent::IsSameWeaponHandType(EEquipmentSlotType EquipmentSlotType, EWeaponHandType WeaponHandType) const
{
	return ((EquipmentSlotType == EEquipmentSlotType::Weapon_Primary_LeftHand  || EquipmentSlotType == EEquipmentSlotType::Weapon_Secondary_LeftHand)  && WeaponHandType == EWeaponHandType::LeftHand  ||
			(EquipmentSlotType == EEquipmentSlotType::Weapon_Primary_RightHand || EquipmentSlotType == EEquipmentSlotType::Weapon_Secondary_RightHand) && WeaponHandType == EWeaponHandType::RightHand ||
			(EquipmentSlotType == EEquipmentSlotType::Weapon_Primary_TwoHand   || EquipmentSlotType == EEquipmentSlotType::Weapon_Secondary_TwoHand)   && WeaponHandType == EWeaponHandType::TwoHand);
}

bool UD1EquipmentManagerComponent::IsSameArmorType(EEquipmentSlotType EquipmentSlotType, EArmorType ArmorType) const
{
	return (EquipmentSlotType == EEquipmentSlotType::Helmet && ArmorType == EArmorType::Helmet || EquipmentSlotType == EEquipmentSlotType::Chest && ArmorType == EArmorType::Chest ||
			EquipmentSlotType == EEquipmentSlotType::Legs && ArmorType == EArmorType::Legs || EquipmentSlotType == EEquipmentSlotType::Hands && ArmorType == EArmorType::Hand ||
			EquipmentSlotType == EEquipmentSlotType::Foot && ArmorType == EArmorType::Foot);
}

bool UD1EquipmentManagerComponent::IsPrimaryWeaponSlot(EEquipmentSlotType EquipmentSlotType) const
{
	return (EquipmentSlotType == EEquipmentSlotType::Weapon_Primary_LeftHand || EquipmentSlotType == EEquipmentSlotType::Weapon_Primary_RightHand || EquipmentSlotType == EEquipmentSlotType::Weapon_Primary_TwoHand);
}

bool UD1EquipmentManagerComponent::IsSecondaryWeaponSlot(EEquipmentSlotType EquipmentSlotType) const
{
	return (EquipmentSlotType == EEquipmentSlotType::Weapon_Secondary_LeftHand || EquipmentSlotType == EEquipmentSlotType::Weapon_Secondary_RightHand || EquipmentSlotType == EEquipmentSlotType::Weapon_Secondary_TwoHand);
}

const TArray<FD1EquipmentEntry>& UD1EquipmentManagerComponent::GetAllEntries() const
{
	return EquipmentList.GetAllEntries();
}

UD1AbilitySystemComponent* UD1EquipmentManagerComponent::GetAbilitySystemComponent() const
{
	return Cast<UD1AbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()));
}
