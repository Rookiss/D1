#include "D1Player.h"

#include "AbilitySystem/D1AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Player/D1PlayerController.h"
#include "Player/D1PlayerState.h"
#include "System/D1AssetManager.h"
#include "UI/D1HUD.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1Player)

AD1Player::AD1Player(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CameraComponent = CreateDefaultSubobject<UCameraComponent>("Camera");
	CameraComponent->SetupAttachment(GetRootComponent());
	CameraComponent->SetRelativeLocation(FVector(0.f, 0.f, 62.f));
	CameraComponent->bUsePawnControlRotation = true;
	
	FirstPersonMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("FirstPersonMeshComponent");
	FirstPersonMeshComponent->SetupAttachment(CameraComponent);
	FirstPersonMeshComponent->SetOwnerNoSee(false);
	FirstPersonMeshComponent->SetOnlyOwnerSee(true);
	FirstPersonMeshComponent->SetRelativeLocation(FVector(-16.6f, -7.1f, -141.4f));
	FirstPersonMeshComponent->SetCastShadow(false);

	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->SetOnlyOwnerSee(false);

	InitialAbilitySystemDataName = "AbilitySystemData_Player";
}

void AD1Player::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// GetMesh()->SetSkeletalMesh(UD1AssetManager::GetAssetByName<USkeletalMesh>("Mannequin_ThirdPerson"));
	// GetMesh()->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	// GetMesh()->SetAnimation(UD1AssetManager::GetAssetByName<UAnimSequence>("ThirdPerson_Idle"));
	//
	// FirstPersonMesh->SetSkeletalMesh(UD1AssetManager::GetAssetByName<USkeletalMesh>("Mannequin_FirstPerson"));
	// FirstPersonMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	// FirstPersonMesh->SetAnimation(UD1AssetManager::GetAssetByName<UAnimSequence>("FirstPerson_Idle"));
}

void AD1Player::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitAbilityActorInfo();
	ApplyAbilitySystemData(InitialAbilitySystemDataName);
}

void AD1Player::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitAbilityActorInfo();
	
	if (AD1PlayerController* D1PC = Cast<AD1PlayerController>(GetController()))
	{
		if (AD1HUD* D1HUD = Cast<AD1HUD>(D1PC->GetHUD()))
		{
			D1HUD->ShowSceneWidget();
		}
	}
}

void AD1Player::InitAbilityActorInfo()
{
	Super::InitAbilityActorInfo();

	if (AD1PlayerState* D1PS = GetPlayerState<AD1PlayerState>())
	{
		D1ASC = Cast<UD1AbilitySystemComponent>(D1PS->GetAbilitySystemComponent());
		check(D1ASC);
		D1ASC->InitAbilityActorInfo(D1PS, this);
	}
}
