#include "D1Player.h"

#include "AbilitySystem/D1AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Item/Managers/D1EquipManagerComponent.h"
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
	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->bUsePawnControlRotation = true;
	CameraComponent->FieldOfView = 100.f;

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -90.f), FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->SetOnlyOwnerSee(false);
	GetMesh()->SetCastHiddenShadow(true);
	
	int32 ArmorTypeCount = static_cast<int32>(EArmorType::Count);
	ArmorMeshComponents.SetNum(ArmorTypeCount);
	ArmorMeshComponents[0] = GetMesh();
	
	for (int32 i = 1; i < ArmorTypeCount; i++)
	{
		USkeletalMeshComponent* ArmorMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(FName(FString::Printf(TEXT("ArmorMeshComponent_%d"), i)));
		ArmorMeshComponent->SetCollisionProfileName("CharacterMesh");
		ArmorMeshComponent->SetGenerateOverlapEvents(true);
		ArmorMeshComponent->SetupAttachment(GetMesh());
		ArmorMeshComponent->SetLeaderPoseComponent(GetMesh());
		ArmorMeshComponents[i] = ArmorMeshComponent;
	}
	
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

	InventoryManagerComponent = CreateDefaultSubobject<UD1InventoryManagerComponent>("InventoryManagerComponent");
	EquipManagerComponent = CreateDefaultSubobject<UD1EquipManagerComponent>("EquipManagerComponent");
	EquipmentManagerComponent = CreateDefaultSubobject<UD1EquipmentManagerComponent>("EquipmentManagerComponent");
}

void AD1Player::BeginPlay()
{
	Super::BeginPlay();

	GetMesh()->SetAnimClass(UD1AssetManager::GetSubclassByName<UAnimInstance>("ABP_Player_Unarmed"));

	TArray<FName> DefaultArmorMeshNames = { "Head_Default", "Chest_Default", "Legs_Default", "Hands_Default", "Foot_Default" };
	DefaultArmorMeshes.SetNum(DefaultArmorMeshNames.Num());
	
	for (int i = 0; i < DefaultArmorMeshNames.Num(); i++)
	{
		DefaultArmorMeshes[i] = UD1AssetManager::GetAssetByName<USkeletalMesh>(DefaultArmorMeshNames[i]);
		ArmorMeshComponents[i]->SetSkeletalMesh(DefaultArmorMeshes[i]);
	}
	
	CameraComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, FName("CameraSocket"));

	if (HasAuthority())
	{
		// @TODO: For Test
		int32 IterationCount = 2;
		const TArray<int32> ItemIDs = { 1001, 1002, 1003 };
	
		for (int i = 0; i < IterationCount; i++)
		{
			for (const int32 ItemID : ItemIDs)
			{
				InventoryManagerComponent->TryAddItem(ItemID, FMath::RandRange(1, 2));
			}
		}
	}
}

void AD1Player::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	InventoryManagerComponent->Init(FIntPoint(10, 5));
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

	EquipmentManagerComponent->Init();
}

void AD1Player::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->RefreshAbilityActorInfo();
	}
	else
	{
		InitAbilitySystem();
	}

	GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
	{
		if (AD1PlayerController* PC = Cast<AD1PlayerController>(GetController()))
		{
			if (AD1HUD* HUD = Cast<AD1HUD>(PC->GetHUD()))
			{
				HUD->ShowSceneWidget();
			}
		}
	});
}

void AD1Player::OnRep_Controller()
{
	Super::OnRep_Controller();

	if (AD1PlayerState* PS = GetPlayerState<AD1PlayerState>())
	{
		PS->GetAbilitySystemComponent()->RefreshAbilityActorInfo();
	}
}

void AD1Player::InitAbilitySystem()
{
	Super::InitAbilitySystem();

	if (AD1PlayerState* PS = GetPlayerState<AD1PlayerState>())
	{
		AbilitySystemComponent = Cast<UD1AbilitySystemComponent>(PS->GetAbilitySystemComponent());
		AbilitySystemComponent->InitAbilityActorInfo(PS, this);
	}
	ApplyAbilitySystemData("ASD_Player");
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

void AD1Player::DisableInputAndCollision()
{
	if (Controller)
	{
		Controller->SetIgnoreMoveInput(true);
	}

	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	MovementComponent->StopMovementImmediately();
	MovementComponent->DisableMovement();
	
	UCapsuleComponent* CollisionComponent = GetCapsuleComponent();
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
}
