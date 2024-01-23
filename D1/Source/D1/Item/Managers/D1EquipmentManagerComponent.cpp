#include "D1EquipmentManagerComponent.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystem/D1AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/D1PlayerSet.h"
#include "Engine/ActorChannel.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1EquipmentManagerComponent)

void FD1EquipmentEntry::Init(UD1ItemInstance* InItemInstance)
{
	if (InItemInstance == nullptr)
		return;

	const UD1ItemFragment_Equippable* Equippable = InItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable>();
	check(Equippable);

	UD1AbilitySystemComponent* ASC = Cast<UD1AbilitySystemComponent>(OwnerComponent->GetAbilitySystemComponent());
	check(ASC);
	
	ItemInstance = InItemInstance;
	LatestValidItemID = ItemInstance->ItemID;
	GrantedHandles.TakeFromAbilitySystem(ASC);
	
	const UD1AbilitySystemData* AbilitySystemData = Equippable->AbilitySystemToGrant;
	AbilitySystemData->GiveToAbilitySystem(ASC, &GrantedHandles, ItemInstance);
	
	APawn* OwningPawn = Cast<APawn>(OwnerComponent->GetOwner());
	check(OwningPawn);
	
	USceneComponent* AttachTarget = OwningPawn->GetRootComponent();
	if (ACharacter* OwningCharacter = Cast<ACharacter>(OwningPawn))
	{
		AttachTarget = OwningCharacter->GetMesh();
	}

	UWorld* World = OwnerComponent->GetWorld();
	const FD1EquipmentAttachInfo& AttachInfo = Equippable->AttachInfo;
	AActor* NewActor = World->SpawnActorDeferred<AActor>(AttachInfo.SpawnActorClass, FTransform::Identity, OwningPawn);
	NewActor->FinishSpawning(FTransform::Identity, true);
	NewActor->SetActorRelativeTransform(AttachInfo.AttachTransform);
	NewActor->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform, AttachInfo.AttachSocket);

	SpawnedActor.Add(NewActor);

	// TODO: Apply Stats
	const FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	const FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(UGameplayEffect::StaticClass(), 1.f, ContextHandle);
	
	const TArray<FD1GameplayTagStack>& Stacks = ItemInstance->GetAllStatStacks();
	for (const FD1GameplayTagStack& Stack : Stacks)
	{
		const FGameplayTag& StatTag = Stack.GetStatTag();
		const int32 StatCount = Stack.GetStatCount();
		SpecHandle.Data->SetSetByCallerMagnitude(StatTag, StatCount);
	}
	FActiveGameplayEffectHandle Handle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	SpecHandle.Data->
}

void FD1EquipmentEntry::Reset()
{
	ItemInstance = nullptr;
	GrantedHandles.TakeFromAbilitySystem(OwnerComponent->GetAbilitySystemComponent());

	for (AActor* Actor : SpawnedActor)
	{
		if (IsValid(Actor))
		{
			Actor->Destroy();
		}
	}
	SpawnedActor.Reset();
	
	// TODO: Revoke Stats
	// ASC->RemoveActiveGameplayEffect();
	
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

		const UD1ItemFragment_Equippable* Equippable = ItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable>();
		if (Equippable == nullptr)
			continue;
		
		if (OwnerComponent->OnEquipmentEntryChanged.IsBound())
		{
			OwnerComponent->OnEquipmentEntryChanged.Broadcast(Equippable->EquipmentType, ItemInstance);
		}
	}
}

void FD1EquipmentList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		const FD1EquipmentEntry& Entry = Entries[Index];

		const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
		const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(Entry.LatestValidItemID);
		
		const UD1ItemFragment_Equippable* Equippable = ItemDef.FindFragmentByClass<UD1ItemFragment_Equippable>();
		if (Equippable == nullptr)
			continue;
		
		if (OwnerComponent->OnEquipmentEntryChanged.IsBound())
		{
			OwnerComponent->OnEquipmentEntryChanged.Broadcast(Equippable->EquipmentType, Entry.ItemInstance);
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

void FD1EquipmentList::Unsafe_EquipItem(UD1ItemInstance* ItemInstance)
{
	const UD1ItemFragment_Equippable* Equippable = ItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable>();
	FD1EquipmentEntry& Entry = Entries[static_cast<int32>(Equippable->EquipmentType)];
	Entry.Init(ItemInstance);
	MarkItemDirty(Entry);
}

void FD1EquipmentList::Unsafe_UnEquipItem(EEquipmentType EquipmentType)
{
	FD1EquipmentEntry& Entry = Entries[static_cast<int32>(EquipmentType)];
	Entry.Reset();
	MarkItemDirty(Entry);
}

bool FD1EquipmentList::CanEquipItem(UD1ItemInstance* ItemInstance) const
{
	if (ItemInstance == nullptr)
		return false;

	const UD1ItemFragment_Equippable* Equippable = ItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable>();
	if (Equippable == nullptr)
		return false;
	
	const FD1EquipmentEntry& Entry = Entries[static_cast<int32>(Equippable->EquipmentType)];
	if (Entry.ItemInstance)
		return false;
	
	return true;
}

bool FD1EquipmentList::CanUnEquipItem(EEquipmentType EquipmentType) const
{
	const FD1EquipmentEntry& Entry = Entries[static_cast<int32>(EquipmentType)];
	return (Entry.ItemInstance != nullptr);
}

FD1EquipmentEntry FD1EquipmentList::GetEntryByType(EEquipmentType EquipmentType) const
{
	return Entries[static_cast<int32>(EquipmentType)];
}

UD1EquipmentManagerComponent::UD1EquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, EquipmentList(this)
{
    SetIsReplicatedByDefault(true);
	bWantsInitializeComponent = true;

	EquipmentList.Entries.Reserve(EquipmentSlotCount);
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

void UD1EquipmentManagerComponent::Server_RequestEquipItem_Implementation(UD1ItemInstance* ItemInstance)
{
	check(GetOwner()->HasAuthority());

	if (CanEquipItem(ItemInstance))
	{
		EquipmentList.Unsafe_EquipItem(ItemInstance);
	}
}

void UD1EquipmentManagerComponent::Server_RequestUnEquipItem_Implementation(EEquipmentType EquipmentType)
{
	check(GetOwner()->HasAuthority());

	if (CanUnEquipItem(EquipmentType))
	{
		EquipmentList.Unsafe_UnEquipItem(EquipmentType);
	}
}

bool UD1EquipmentManagerComponent::CanEquipItem(UD1ItemInstance* ItemInstance) const
{
	return EquipmentList.CanEquipItem(ItemInstance);
}

bool UD1EquipmentManagerComponent::CanUnEquipItem(EEquipmentType EquipmentType) const
{
	return EquipmentList.CanUnEquipItem(EquipmentType);
}

const TArray<FD1EquipmentEntry>& UD1EquipmentManagerComponent::GetAllEntries() const
{
	return EquipmentList.GetAllEntries();
}

FD1EquipmentEntry UD1EquipmentManagerComponent::GetEntryByType(EEquipmentType EquipmentType)
{
	return EquipmentList.GetEntryByType(EquipmentType);
}

UD1AbilitySystemComponent* UD1EquipmentManagerComponent::GetAbilitySystemComponent() const
{
	return Cast<UD1AbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()));
}
