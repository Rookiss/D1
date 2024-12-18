#include "D1EquipmentBase.h"

#include "AbilitySystemComponent.h"
#include "D1GameplayTags.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Character/LyraCharacter.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Data/D1ItemData.h"
#include "Item/D1ItemInstance.h"
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
	
	DOREPLIFETIME(ThisClass, ItemTemplateID);
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
	
	ItemTemplateID = InTemplateID;
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

void AD1EquipmentBase::OnRep_ItemTemplateID()
{
	CheckPropertyInitialization();
}

void AD1EquipmentBase::OnRep_EquipmentSlotType()
{
	CheckPropertyInitialization();
}

UAbilitySystemComponent* AD1EquipmentBase::GetAbilitySystemComponent() const
{
	if (ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(GetOwner()))
	{
		return LyraCharacter->GetAbilitySystemComponent();
	}
	
	return nullptr;
}

void AD1EquipmentBase::PlayEquipMontage()
{
	ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(GetOwner());
	if (LyraCharacter == nullptr)
		return;
	
	UAbilitySystemComponent* ASC = LyraCharacter->GetAbilitySystemComponent();
	if (ASC == nullptr)
		return;

	check(ItemTemplateID > 0);
	
	const UD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(ItemTemplateID);
	const UD1ItemFragment_Equipable_Attachment* AttachmentFragment = ItemTemplate.FindFragmentByClass<UD1ItemFragment_Equipable_Attachment>();
	if (AttachmentFragment == nullptr)
		return;

	if (USkeletalMeshComponent* CharacterMeshComponent = LyraCharacter->GetMesh())
	{
		if (AttachmentFragment->AnimInstanceClass)
		{
			CharacterMeshComponent->LinkAnimClassLayers(AttachmentFragment->AnimInstanceClass);
		}
		
		if (ASC->HasMatchingGameplayTag(D1GameplayTags::Status_Interact) == false)
		{
			UAnimMontage* EquipMontage = ULyraAssetManager::GetAssetByPath<UAnimMontage>(GetEquipMontage(ProcessEquip()));
			if (UAnimInstance* AnimInstance = CharacterMeshComponent->GetAnimInstance())
			{
				if (AnimInstance->GetCurrentActiveMontage() != EquipMontage)
				{
					LyraCharacter->PlayAnimMontage(EquipMontage);
				}
			}
		}
	}
}

void AD1EquipmentBase::CheckPropertyInitialization()
{
	if (bOnlyUseForLocal)
		return;

	if (ItemTemplateID <= 0 || EquipmentSlotType == EEquipmentSlotType::Count)
		return;
	
	if (GetOwner())
	{
		UD1EquipManagerComponent* EquipManager = GetOwner()->FindComponentByClass<UD1EquipManagerComponent>();
		UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
		if (EquipManager && ASC)
		{
			TArray<FD1EquipEntry>& Entries = EquipManager->GetAllEntries();
			Entries[(int32)EquipmentSlotType].SetEquipmentActor(this);
			PlayEquipMontage();
			return;
		}
	}
	
	GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::CheckPropertyInitialization);
}

FGameplayTagContainer AD1EquipmentBase::ProcessEquip_Implementation() const
{
	return FGameplayTagContainer();
}

TSoftObjectPtr<UAnimMontage> AD1EquipmentBase::GetEquipMontage(const FGameplayTagContainer& ContextTags)
{
	for (const FD1EquipStyle& EquipStyle : EquipStyles)
	{
		if (EquipStyle.MatchPattern.Matches(ContextTags) || EquipStyle.MatchPattern.IsEmpty())
		{
			return EquipStyle.EquipMontage;
		}
	}

	return nullptr;
}

UAnimMontage* AD1EquipmentBase::GetHitMontage(AActor* InstigatorActor, const FVector& HitLocation, bool IsBlocked)
{
	UAnimMontage* SelectedMontage = nullptr;
	
	if (InstigatorActor && ItemTemplateID > 0)
	{
		const UD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(ItemTemplateID);
		if (const UD1ItemFragment_Equipable_Attachment* AttachmentFragment = ItemTemplate.FindFragmentByClass<UD1ItemFragment_Equipable_Attachment>())
		{
			TSoftObjectPtr<UAnimMontage> SelectedMontagePath = nullptr;
			
			if (IsBlocked)
			{
				SelectedMontagePath = AttachmentFragment->BlockHitMontage;
			}
			else
			{
				AActor* CharacterActor = GetOwner();
				FVector CharacterLocation = CharacterActor->GetActorLocation();
				FVector CharacterDirection = CharacterActor->GetActorForwardVector();
			
				FRotator FacingRotator = UKismetMathLibrary::Conv_VectorToRotator(CharacterDirection);
				FRotator CharacterToHitRotator = UKismetMathLibrary::Conv_VectorToRotator((HitLocation - CharacterLocation).GetSafeNormal());
			
				FRotator DeltaRotator = UKismetMathLibrary::NormalizedDeltaRotator(CharacterToHitRotator, FacingRotator);
				float YawAbs = FMath::Abs(DeltaRotator.Yaw);
				
				if (YawAbs < 60.f)
				{
					SelectedMontagePath = AttachmentFragment->FrontHitMontage;
				}
				else if (YawAbs > 120.f)
				{
					SelectedMontagePath = AttachmentFragment->BackHitMontage;
				}
				else if (DeltaRotator.Yaw < 0.f)
				{
					SelectedMontagePath = AttachmentFragment->LeftHitMontage;
				}
				else
				{
					SelectedMontagePath = AttachmentFragment->RightHitMontage;
				}
			}

			if (SelectedMontagePath.IsNull() == false)
			{
				SelectedMontage = ULyraAssetManager::GetAssetByPath<UAnimMontage>(SelectedMontagePath);
			}
		}
	}
	
	return SelectedMontage;
}
