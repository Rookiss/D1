#include "D1Player.h"

#include "AbilitySystem/D1AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Item/Managers/D1EquipmentManagerComponent.h"
#include "Item/Managers/D1InventoryManagerComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Player/D1PlayerController.h"
#include "Player/D1PlayerState.h"
#include "System/D1AssetManager.h"
#include "UI/D1HUD.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1Player)

AD1Player::AD1Player(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	EquipmentManagerComponent = CreateDefaultSubobject<UD1EquipmentManagerComponent>("EquipmentManagerComponent");
	InventoryManagerComponent = CreateDefaultSubobject<UD1InventoryManagerComponent>("InventoryManagerComponent");
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->bUsePawnControlRotation = true;

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -90.f), FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->SetOnlyOwnerSee(false);
	GetMesh()->SetCastHiddenShadow(true);

	WeaponLeftMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponLeftMeshComponent");
	WeaponLeftMeshComponent->SetupAttachment(GetMesh());

	WeaponRightMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponRightMeshComponent");
	WeaponRightMeshComponent->SetupAttachment(GetMesh());

	HelmetMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("HelmetMeshComponent");
	HelmetMeshComponent->SetupAttachment(GetMesh());

	ChestMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("ChestMeshComponent");
	ChestMeshComponent->SetupAttachment(GetMesh());

	LegsMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("LegsMeshComponent");
	LegsMeshComponent->SetupAttachment(GetMesh());

	HandsMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("HandsMeshComponent");
	HandsMeshComponent->SetupAttachment(GetMesh());

	FootMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("FootMeshComponent");
	FootMeshComponent->SetupAttachment(GetMesh());

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

	HelmetMeshComponent->SetLeaderPoseComponent(GetMesh());
	ChestMeshComponent->SetLeaderPoseComponent(GetMesh());
	LegsMeshComponent->SetLeaderPoseComponent(GetMesh());
	HandsMeshComponent->SetLeaderPoseComponent(GetMesh());
	FootMeshComponent->SetLeaderPoseComponent(GetMesh());
}

void AD1Player::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	InventoryManagerComponent->Init(FIntPoint(10, 5));
	
	GetMesh()->SetAnimClass(UD1AssetManager::GetSubclassByName<UAnimInstance>("ABP_Player"));
	
	CameraComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("CameraSocket"));
	
	GetMesh()->SetSkeletalMesh(UD1AssetManager::GetAssetByName<USkeletalMesh>("Head_Default"));
	ChestMeshComponent->SetSkeletalMesh(UD1AssetManager::GetAssetByName<USkeletalMesh>("Chest_Default"));
	LegsMeshComponent->SetSkeletalMesh(UD1AssetManager::GetAssetByName<USkeletalMesh>("Legs_Default"));
	HandsMeshComponent->SetSkeletalMesh(UD1AssetManager::GetAssetByName<USkeletalMesh>("Hands_Default"));
	FootMeshComponent->SetSkeletalMesh(UD1AssetManager::GetAssetByName<USkeletalMesh>("Foot_Default"));
}

void AD1Player::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, TargetAimPitch, COND_SkipOwner);
}

void AD1Player::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitAbilitySystem();

	// @TODO: For Test
	int32 IterateCount = 2;
	TArray<int32> IDs = { 1001, 2001, 3001, 9999 };
	for (int i = 0; i < IterateCount; i++)
	{
		for (int32 j = 0; j < IDs.Num(); j++)
		{
			InventoryManagerComponent->TryAddItem(IDs[j], FMath::RandRange(1, 2));
		}
	}
}

void AD1Player::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitAbilitySystem();
	
	if (AD1PlayerController* PC = Cast<AD1PlayerController>(GetController()))
	{
		if (AD1HUD* HUD = Cast<AD1HUD>(PC->GetHUD()))
		{
			HUD->ShowSceneWidget();
		}
	}
}

void AD1Player::InitAbilitySystem()
{
	Super::InitAbilitySystem();

	if (AD1PlayerState* PS = GetPlayerState<AD1PlayerState>())
	{
		AbilitySystemComponent = Cast<UD1AbilitySystemComponent>(PS->GetAbilitySystemComponent());
		check(AbilitySystemComponent);
		AbilitySystemComponent->InitAbilityActorInfo(PS, this);
	}
	ApplyAbilitySystemData("AbilitySystemData_Player");
}

float AD1Player::CalculateAimPitch()
{
	if (IsLocallyControlled() || HasAuthority())
	{
		float Pitch = (GetControlRotation() - GetActorRotation()).GetNormalized().Pitch;
		TargetAimPitch = UKismetMathLibrary::ClampAngle(Pitch, -90.f, 90.f);
		return TargetAimPitch;
	}
	else
	{
		CurrentAimPitch = FMath::FInterpTo(CurrentAimPitch, TargetAimPitch, GetWorld()->DeltaTimeSeconds, 12.f);
		return CurrentAimPitch;
	}
}
