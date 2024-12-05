#include "D1EquipmentBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Character/LyraCharacter.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Data/D1ItemData.h"
#include "Item/Fragments/D1ItemFragment_Equipable_Weapon.h"
#include "Item/Managers/D1EquipManagerComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Physics/LyraCollisionChannels.h"
#include "System/LyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1EquipmentBase)

AD1EquipmentBase::AD1EquipmentBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	
    bReplicates = true;

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>("ArrowComponent");
	ArrowComponent->PrimaryComponentTick.bStartWithTickEnabled = false;
	SetRootComponent(ArrowComponent);
	
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	MeshComponent->SetCollisionProfileName("Weapon");
	MeshComponent->SetGenerateOverlapEvents(false);
	MeshComponent->SetupAttachment(GetRootComponent());
	MeshComponent->PrimaryComponentTick.bStartWithTickEnabled = false;
	MeshComponent->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	
	TraceDebugCollision = CreateDefaultSubobject<UBoxComponent>("TraceDebugCollision");
	TraceDebugCollision->SetCollisionProfileName("NoCollision");
	TraceDebugCollision->SetGenerateOverlapEvents(false);
	TraceDebugCollision->SetupAttachment(GetRootComponent());
	TraceDebugCollision->PrimaryComponentTick.bStartWithTickEnabled = false;
}

void AD1EquipmentBase::BeginPlay()
{
	Super::BeginPlay();

	if (bOnlyUseForLocal)
		return;
	
	if (HasAuthority())
	{
		OnRep_EquipmentSlotType();
	}
}

void AD1EquipmentBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	if (bOnlyUseForLocal)
		return;
	
	DOREPLIFETIME(ThisClass, TemplateID);
	DOREPLIFETIME(ThisClass, EquipmentSlotType);
	DOREPLIFETIME(ThisClass, bCanBlock);
}

void AD1EquipmentBase::Destroyed()
{
	if (bOnlyUseForLocal)
		return;
	
	if (ALyraCharacter* Character = Cast<ALyraCharacter>(GetOwner()))
	{
		if (ULyraAbilitySystemComponent* ASC = Cast<ULyraAbilitySystemComponent>(Character->GetAbilitySystemComponent()))
		{
			SkillAbilitySetHandles.TakeFromAbilitySystem(ASC);
		}

		if (UD1EquipManagerComponent* EquipManager = Character->FindComponentByClass<UD1EquipManagerComponent>())
		{
			TArray<FD1EquipEntry>& Entries = EquipManager->GetAllEntries();
			if (Entries[(int32)EquipmentSlotType].GetEquipmentActor() == this)
			{
				Entries[(int32)EquipmentSlotType].SetEquipmentActor(nullptr);
			}
		}
	}
	
	Super::Destroyed();
}

void AD1EquipmentBase::Init(int32 InTemplateID, EEquipmentSlotType InEquipmentSlotType)
{
	if (bOnlyUseForLocal)
		return;
	
	TemplateID = InTemplateID;
	EquipmentSlotType = InEquipmentSlotType;
}

void AD1EquipmentBase::ChangeBlockState(bool bShouldBlock)
{
	if (bOnlyUseForLocal)
		return;
	
	if (HasAuthority())
	{
		bCanBlock = bShouldBlock;
		OnRep_CanBlock();
	}
}

void AD1EquipmentBase::OnRep_CanBlock()
{
	if (bOnlyUseForLocal)
		return;
	
	MeshComponent->SetCollisionResponseToChannel(D1_ObjectChannel_Projectile, bCanBlock ? ECR_Block : ECR_Ignore);
}

void AD1EquipmentBase::OnRep_EquipmentSlotType()
{
	if (bOnlyUseForLocal)
		return;
	
	if (GetOwner() && GetOwner()->FindComponentByClass<UD1EquipManagerComponent>())
	{
		if (ALyraCharacter* Character = Cast<ALyraCharacter>(GetOwner()))
		{
			if (UD1EquipManagerComponent* EquipManager = Character->FindComponentByClass<UD1EquipManagerComponent>())
			{
				TArray<FD1EquipEntry>& Entries = EquipManager->GetAllEntries();
				Entries[(int32)EquipmentSlotType].SetEquipmentActor(this);
			}
		}
	}
	else
	{
		GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::OnRep_EquipmentSlotType);
	}
}

UAbilitySystemComponent* AD1EquipmentBase::GetAbilitySystemComponent() const
{
	UAbilitySystemComponent* ASC = nullptr;
	if (ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(GetOwner()))
	{
		ASC = LyraCharacter->GetAbilitySystemComponent();
	}
	return ASC;
}

UAnimMontage* AD1EquipmentBase::GetEquipMontage()
{
	UAnimMontage* EquipMontage = nullptr;
	
	if (TemplateID > 0)
	{
		const UD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(TemplateID);
		if (const UD1ItemFragment_Equipable_Attachment* AttachmentFragment = ItemTemplate.FindFragmentByClass<UD1ItemFragment_Equipable_Attachment>())
		{
			EquipMontage = ULyraAssetManager::GetAssetByPath<UAnimMontage>(AttachmentFragment->EquipMontage);
		}
	}
	
	return EquipMontage;
}

UAnimMontage* AD1EquipmentBase::GetHitMontage(AActor* InstigatorActor, const FVector& HitLocation, bool IsBlocked)
{
	UAnimMontage* SelectedMontage = nullptr;
	
	if (InstigatorActor && TemplateID > 0)
	{
		const UD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(TemplateID);
		if (const UD1ItemFragment_Equipable_Attachment* AttachmentFragment = ItemTemplate.FindFragmentByClass<UD1ItemFragment_Equipable_Attachment>())
		{
			if (IsBlocked)
			{
				SelectedMontage = ULyraAssetManager::GetAssetByPath<UAnimMontage>(AttachmentFragment->BlockHitMontage);
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
					SelectedMontage = ULyraAssetManager::GetAssetByPath<UAnimMontage>(AttachmentFragment->FrontHitMontage);
				}
				else if (YawAbs > 120.f)
				{
					SelectedMontage = ULyraAssetManager::GetAssetByPath<UAnimMontage>(AttachmentFragment->BackHitMontage);
				}
				else if (DeltaRotator.Yaw < 0.f)
				{
					SelectedMontage = ULyraAssetManager::GetAssetByPath<UAnimMontage>(AttachmentFragment->LeftHitMontage);
				}
				else
				{
					SelectedMontage = ULyraAssetManager::GetAssetByPath<UAnimMontage>(AttachmentFragment->RightHitMontage);
				}
			}
		}
	}
	
	return SelectedMontage;
}
