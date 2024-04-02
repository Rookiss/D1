#include "D1WeaponBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/D1AbilitySystemComponent.h"
#include "AbilitySystem/Abilities/D1GameplayAbility_Weapon.h"
#include "Character/D1Player.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Data/D1ItemData.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Weapon.h"
#include "Item/Managers/D1EquipManagerComponent.h"
#include "Net/UnrealNetwork.h"
#include "System/D1AssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1WeaponBase)

AD1WeaponBase::AD1WeaponBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>("ArrowComponent");
	SetRootComponent(ArrowComponent);
	
	WeaponMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("WeaponMesh");
	WeaponMeshComponent->SetupAttachment(GetRootComponent());
	WeaponMeshComponent->SetCollisionProfileName("Weapon");
	WeaponMeshComponent->SetGenerateOverlapEvents(false);
	
	TraceDebugCollision = CreateDefaultSubobject<UBoxComponent>("TraceDebugCollision");
	TraceDebugCollision->SetupAttachment(GetRootComponent());
	TraceDebugCollision->SetCollisionProfileName("NoCollision");
	TraceDebugCollision->SetGenerateOverlapEvents(false);
}

void AD1WeaponBase::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		OnRep_TemplateID();
		OnRep_EquipmentSlotType();
	}
}

void AD1WeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, TemplateID);
	DOREPLIFETIME(ThisClass, EquipmentSlotType);
	DOREPLIFETIME(ThisClass, bCanBlock);
}

void AD1WeaponBase::Destroyed()
{
	if (AD1Player* Player = Cast<AD1Player>(GetOwner()))
	{
		if (UD1EquipManagerComponent* EquipManager = Player->EquipManagerComponent)
		{
			TArray<FD1EquipEntry>& Entries = EquipManager->GetAllEntries();
			if (Entries[(int32)EquipmentSlotType].SpawnedWeaponActor == this)
			{
				Entries[(int32)EquipmentSlotType].SpawnedWeaponActor = nullptr;
			}
		}
	}

	if (SkillAbilitySpecHandle.IsValid())
	{
		if (AD1Character* Character = Cast<AD1Character>(GetOwner()))
		{
			if (UD1AbilitySystemComponent* ASC = Character->GetD1AbilitySystemComponent())
			{
				ASC->ClearAbility(SkillAbilitySpecHandle);
			}
		}
	}
	
	Super::Destroyed();
}

void AD1WeaponBase::ChangeSkill(int32 AbilityIndex)
{
	if (HasAuthority() == false)
		return;
	
	AD1Character* Character = Cast<AD1Character>(GetOwner());
	check(Character);
	
	UD1AbilitySystemComponent* ASC = Character->GetD1AbilitySystemComponent();
	check(ASC);
	
	if (SkillAbilitySpecHandle.IsValid())
	{
		ASC->ClearAbility(SkillAbilitySpecHandle);
		SkillAbilitySpecHandle = FGameplayAbilitySpecHandle();
	}

	const FD1ItemTemplate& ItemTemplate = UD1AssetManager::GetItemTemplate(TemplateID);
	if (const UD1ItemFragment_Equippable_Weapon* WeaponFragment = ItemTemplate.FindFragmentByClass<UD1ItemFragment_Equippable_Weapon>())
	{
		if (const UD1AbilitySystemData* AbilitySystemData = WeaponFragment->AbilitySystemData)
		{
			AbilitySystemData->GivePendingAbility(ASC, &SkillAbilitySpecHandle, AbilityIndex);
		}
	}
}

void AD1WeaponBase::OnRep_TemplateID()
{
	const FD1ItemTemplate& ItemTemplate = UD1AssetManager::GetItemTemplate(TemplateID);
	const UD1ItemFragment_Equippable_Weapon* Weapon = ItemTemplate.FindFragmentByClass<UD1ItemFragment_Equippable_Weapon>();

	if (AD1Character* Character = Cast<AD1Character>(GetOwner()))
	{
		if (Character->GetMesh()->GetAnimClass() != Weapon->AnimInstanceClass)
		{
			Character->GetMesh()->SetAnimClass(Weapon->AnimInstanceClass);
		}

		UD1AssetManager::GetAssetByPath(Weapon->EquipMontage.ToSoftObjectPath(), FAsyncLoadCompletedDelegate::CreateLambda(
		[Character, this](const FName& AssetName, UObject* LoadedAsset)
		{
			Character->PlayAnimMontage(Cast<UAnimMontage>(LoadedAsset));
		}));
	}
}

void AD1WeaponBase::OnRep_EquipmentSlotType()
{
	if (AD1Player* Player = Cast<AD1Player>(GetOwner()))
	{
		if (UD1EquipManagerComponent* EquipManager = Player->EquipManagerComponent)
		{
			TArray<FD1EquipEntry>& Entries = EquipManager->GetAllEntries();
			Entries[(int32)EquipmentSlotType].SpawnedWeaponActor = this;
		}
	}
}
