#include "D1WeaponBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Character/LyraCharacter.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Data/D1ItemData.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Weapon.h"
#include "Item/Managers/D1EquipManagerComponent.h"
#include "Net/UnrealNetwork.h"
#include "Physics/LyraCollisionChannels.h"
#include "System/LyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1WeaponBase)

AD1WeaponBase::AD1WeaponBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>("ArrowComponent");
	SetRootComponent(ArrowComponent);
	
	WeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	WeaponMeshComponent->SetCollisionProfileName("Weapon");
	WeaponMeshComponent->SetGenerateOverlapEvents(false);
	WeaponMeshComponent->SetHiddenInGame(true);
	WeaponMeshComponent->SetupAttachment(GetRootComponent());
	
	TraceDebugCollision = CreateDefaultSubobject<UBoxComponent>("TraceDebugCollision");
	TraceDebugCollision->SetCollisionProfileName("NoCollision");
	TraceDebugCollision->SetGenerateOverlapEvents(false);
	TraceDebugCollision->SetupAttachment(GetRootComponent());
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
	if (ALyraCharacter* Character = Cast<ALyraCharacter>(GetOwner()))
	{
		if (ULyraAbilitySystemComponent* ASC = Cast<ULyraAbilitySystemComponent>(Character->GetAbilitySystemComponent()))
		{
			SkillAbilitySetHandles.TakeFromAbilitySystem(ASC);
		}

		if (UD1EquipManagerComponent* EquipManager = Character->FindComponentByClass<UD1EquipManagerComponent>())
		{
			TArray<FD1EquipEntry>& Entries = EquipManager->GetAllEntries();
			if (Entries[(int32)EquipmentSlotType].SpawnedWeaponActor == this)
			{
				Entries[(int32)EquipmentSlotType].SpawnedWeaponActor = nullptr;
			}
		}
	}
	
	Super::Destroyed();
}

void AD1WeaponBase::Init(int32 InTemplateID, EEquipmentSlotType InEquipmentSlotType)
{
	TemplateID = InTemplateID;
	EquipmentSlotType = InEquipmentSlotType;
}

void AD1WeaponBase::ChangeSkill(int32 AbilitySetIndex)
{
	if (HasAuthority() == false)
		return;
	
	ALyraCharacter* Character = Cast<ALyraCharacter>(GetOwner());
	check(Character);
	
	ULyraAbilitySystemComponent* ASC = Cast<ULyraAbilitySystemComponent>(Character->GetAbilitySystemComponent());
	check(ASC);

	SkillAbilitySetHandles.TakeFromAbilitySystem(ASC);

	const FD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(TemplateID);
	if (const UD1ItemFragment_Equippable_Weapon* WeaponFragment = ItemTemplate.FindFragmentByClass<UD1ItemFragment_Equippable_Weapon>())
	{
		if (WeaponFragment->SkillAbilitySets.IsValidIndex(AbilitySetIndex))
		{
			if (const ULyraAbilitySet* SkillAbilitySet = WeaponFragment->SkillAbilitySets[AbilitySetIndex])
			{
				SkillAbilitySet->GiveToAbilitySystem(ASC, &SkillAbilitySetHandles, this);
			}
		}
	}
}

void AD1WeaponBase::ChangeBlockState(bool bShouldBlock)
{
	bCanBlock = bShouldBlock;
	WeaponMeshComponent->SetCollisionResponseToChannel(D1_ObjectChannel_Projectile, bShouldBlock ? ECR_Block : ECR_Ignore);
}

void AD1WeaponBase::OnRep_CanBlock()
{
	WeaponMeshComponent->SetCollisionResponseToChannel(D1_ObjectChannel_Projectile, bCanBlock ? ECR_Block : ECR_Ignore);
}

void AD1WeaponBase::OnRep_TemplateID()
{
	const FD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(TemplateID);
	const UD1ItemFragment_Equippable_Weapon* WeaponFragment = ItemTemplate.FindFragmentByClass<UD1ItemFragment_Equippable_Weapon>();
	check(WeaponFragment);
	
	if (ALyraCharacter* Character = Cast<ALyraCharacter>(GetOwner()))
	{
		if (USkeletalMeshComponent* MeshComponent = Character->GetMesh())
		{
			if (WeaponFragment->AnimInstanceClass)
			{
				MeshComponent->LinkAnimClassLayers(WeaponFragment->AnimInstanceClass);
			}

			UAnimMontage* EquipMontage = ULyraAssetManager::GetAssetByPath<UAnimMontage>(WeaponFragment->EquipMontage);
			if (UAnimInstance* AnimInstance = MeshComponent->GetAnimInstance())
			{
				if (AnimInstance->GetCurrentActiveMontage() != EquipMontage)
				{
					Character->PlayAnimMontage(EquipMontage);
				}
			}
		}
		
		WeaponMeshComponent->SetHiddenInGame(false);
	}
}

void AD1WeaponBase::OnRep_EquipmentSlotType()
{
	if (ALyraCharacter* Character = Cast<ALyraCharacter>(GetOwner()))
	{
		if (UD1EquipManagerComponent* EquipManager = Character->FindComponentByClass<UD1EquipManagerComponent>())
		{
			TArray<FD1EquipEntry>& Entries = EquipManager->GetAllEntries();
			Entries[(int32)EquipmentSlotType].SpawnedWeaponActor = this;
		}
	}
}

UAbilitySystemComponent* AD1WeaponBase::GetAbilitySystemComponent() const
{
	return Cast<ALyraCharacter>(GetOwner())->GetAbilitySystemComponent();
}
