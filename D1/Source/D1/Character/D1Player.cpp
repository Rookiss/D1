#include "D1Player.h"

#include "AbilitySystem/D1AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Data/D1ItemData.h"
#include "GameFramework/CharacterMovementComponent.h"
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

	int32 ArmorTypeCount = static_cast<int32>(EArmorType::Count);
	ArmorMeshComponents.SetNum(ArmorTypeCount);
	
	for (int32 i = 0; i < ArmorTypeCount; i++)
	{
		USkeletalMeshComponent* ArmorMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(FName(FString::Printf(TEXT("ArmorMeshComponent_%d"), i)));
		ArmorMeshComponent->SetupAttachment(GetMesh());
		ArmorMeshComponent->SetLeaderPoseComponent(GetMesh());
		ArmorMeshComponents[i] = ArmorMeshComponent;
	}
	
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
}

void AD1Player::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	InventoryManagerComponent->Init(FIntPoint(10, 5));
}

void AD1Player::BeginPlay()
{
	Super::BeginPlay();
	
	GetMesh()->SetSkeletalMesh(UD1AssetManager::GetAssetByName<USkeletalMesh>("Head_Default"));
	GetMesh()->SetAnimClass(UD1AssetManager::GetSubclassByName<UAnimInstance>("ABP_Player"));
	
	CameraComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("CameraSocket"));

	TArray<FName> DefaultArmorMeshNames = { "None", "Chest_Default", "Legs_Default", "Hands_Default", "Foot_Default" };
	DefaultArmorMeshes.SetNum(DefaultArmorMeshNames.Num());
	
	for (int i = 0; i < DefaultArmorMeshNames.Num(); i++)
	{
		if (DefaultArmorMeshNames[i] == "None")
			continue;
		
		DefaultArmorMeshes[i] = UD1AssetManager::GetAssetByName<USkeletalMesh>(DefaultArmorMeshNames[i]);
		ArmorMeshComponents[i]->SetSkeletalMesh(DefaultArmorMeshes[i]);
	}

	if (HasAuthority())
	{
		// @TODO: For Test
		int32 IterateCount = 2;
		UD1ItemData* ItemData = UD1AssetManager::GetItemData();
		const TMap<int32, FD1ItemDefinition>& AllItemDefs = ItemData->GetAllItemDefs();
	
		for (int i = 0; i < IterateCount; i++)
		{
			for (const auto& Pair : AllItemDefs)
			{
				InventoryManagerComponent->TryAddItem(Pair.Key, FMath::RandRange(1, 2));
			}
		}
	}
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

void AD1Player::DisableInputAndCollision()
{
	if (Controller)
	{
		Controller->SetIgnoreMoveInput(true);
	}

	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	check(MovementComponent);
	MovementComponent->StopMovementImmediately();
	MovementComponent->DisableMovement();
	
	UCapsuleComponent* CollisionComponent = GetCapsuleComponent();
	check(CollisionComponent);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
}

void AD1Player::Multicast_SetArmorMesh_Implementation(EArmorType ArmorType, FSoftObjectPath ArmorMeshPath)
{
	if (ArmorType == EArmorType::Count)
		return;

	if (ArmorMeshPath.IsValid())
	{
		UD1AssetManager::GetAssetByPath(ArmorMeshPath, FAsyncLoadCompletedDelegate::CreateLambda(
		[this, ArmorType](const FName& AssetName, UObject* LoadedAsset)
		{
			if (USkeletalMeshComponent* ArmorMeshComponent = ArmorMeshComponents[static_cast<int32>(ArmorType)])
			{
				ArmorMeshComponent->SetSkeletalMesh(Cast<USkeletalMesh>(LoadedAsset));
			}
		}));
	}
	else
	{
		if (USkeletalMeshComponent* ArmorMeshComponent = ArmorMeshComponents[static_cast<int32>(ArmorType)])
		{
			ArmorMeshComponent->SetSkeletalMesh(DefaultArmorMeshes[static_cast<int32>(ArmorType)]);
		}
	}
}
