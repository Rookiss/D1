#include "D1WeaponBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Character/LyraCharacter.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Data/D1ItemData.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Weapon.h"
#include "Item/Managers/D1EquipManagerComponent.h"
#include "Kismet/KismetMathLibrary.h"
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
	DOREPLIFETIME(ThisClass, bShouldHidden);
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

	const UD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(TemplateID);
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
	OnRep_CanBlock();
}

void AD1WeaponBase::ChangeVisibilityState(bool bNewShouldHidden)
{
	bShouldHidden = bNewShouldHidden;
	OnRep_ShouldHidden();
}

void AD1WeaponBase::OnRep_CanBlock()
{
	WeaponMeshComponent->SetCollisionResponseToChannel(D1_ObjectChannel_Weapon, bCanBlock ? ECR_Overlap : ECR_Ignore);
	WeaponMeshComponent->SetCollisionResponseToChannel(D1_ObjectChannel_Projectile, bCanBlock ? ECR_Block : ECR_Ignore);
}

void AD1WeaponBase::OnRep_ShouldHidden()
{
	TArray<UMeshComponent*> MeshComponents;
	GetComponents(UMeshComponent::StaticClass(), MeshComponents);
	
	for (UMeshComponent* MeshComponent : MeshComponents)
	{
		if (MeshComponent)
		{
			MeshComponent->SetHiddenInGame(bShouldHidden);
		}
	}
}

void AD1WeaponBase::OnRep_TemplateID()
{
	WeaponMeshComponent->SetHiddenInGame(false);
}

void AD1WeaponBase::OnRep_EquipmentSlotType()
{
	if (GetOwner() && GetOwner()->FindComponentByClass<UD1EquipManagerComponent>())
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
	else
	{
		GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::OnRep_EquipmentSlotType);
	}
}

UAbilitySystemComponent* AD1WeaponBase::GetAbilitySystemComponent() const
{
	return Cast<ALyraCharacter>(GetOwner())->GetAbilitySystemComponent();
}

UAnimMontage* AD1WeaponBase::GetEquipMontage()
{
	UAnimMontage* EquipMontage = nullptr;
	
	if (TemplateID > 0)
	{
		const UD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(TemplateID);
		if (const UD1ItemFragment_Equippable_Weapon* WeaponFragment = ItemTemplate.FindFragmentByClass<UD1ItemFragment_Equippable_Weapon>())
		{
			EquipMontage = ULyraAssetManager::GetAssetByPath<UAnimMontage>(WeaponFragment->EquipMontage);
		}
	}
	
	return EquipMontage;
}

UAnimMontage* AD1WeaponBase::GetHitMontage(AActor* InstigatorActor, const FVector& HitLocation, bool IsBlocked)
{
	UAnimMontage* SelectedMontage = nullptr;
	
	if (InstigatorActor && TemplateID > 0)
	{
		const UD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(TemplateID);
		if (const UD1ItemFragment_Equippable_Weapon* WeaponFragment = ItemTemplate.FindFragmentByClass<UD1ItemFragment_Equippable_Weapon>())
		{
			if (IsBlocked)
			{
				SelectedMontage = ULyraAssetManager::GetAssetByPath<UAnimMontage>(WeaponFragment->BlockHitMontage);
			}
			else
			{
				AActor* CharacterActor = GetOwner();
				const FVector& CharacterLocation = CharacterActor->GetActorLocation();
				const FVector& CharacterDirection = CharacterActor->GetActorForwardVector();
			
				const FRotator& FacingRotator = UKismetMathLibrary::Conv_VectorToRotator(CharacterDirection);
				const FRotator& CharacterToHitRotator = UKismetMathLibrary::Conv_VectorToRotator((HitLocation - CharacterLocation).GetSafeNormal());
			
				const FRotator& DeltaRotator = UKismetMathLibrary::NormalizedDeltaRotator(CharacterToHitRotator, FacingRotator);
				float YawAbs = FMath::Abs(DeltaRotator.Yaw);

				if (YawAbs < 60.f)
				{
					SelectedMontage = ULyraAssetManager::GetAssetByPath<UAnimMontage>(WeaponFragment->FrontHitMontage);
				}
				else if (YawAbs > 120.f)
				{
					SelectedMontage = ULyraAssetManager::GetAssetByPath<UAnimMontage>(WeaponFragment->BackHitMontage);
				}
				else if (DeltaRotator.Yaw < 0.f)
				{
					SelectedMontage = ULyraAssetManager::GetAssetByPath<UAnimMontage>(WeaponFragment->LeftHitMontage);
				}
				else
				{
					SelectedMontage = ULyraAssetManager::GetAssetByPath<UAnimMontage>(WeaponFragment->RightHitMontage);
				}
			}
		}
	}
	
	return SelectedMontage;
}
