#include "D1Character.h"

#include "D1GameplayTags.h"
#include "D1Player.h"
#include "AbilitySystem/D1AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/D1AttributeSet.h"
#include "Animation/D1AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Data/D1AbilitySystemData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "System/D1AssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1Character)

AD1Character::AD1Character(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NetCullDistanceSquared = 900000000.0f;
	
	GetCapsuleComponent()->InitCapsuleSize(44.0f, 88.0f);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetGenerateOverlapEvents(true);
	GetMesh()->SetCollisionProfileName("CharacterMesh");
	
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = false;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
	GetCharacterMovement()->SetCrouchedHalfHeight(65.0f);
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 150.f;
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	BaseEyeHeight = 80.0f;
	CrouchedEyeHeight = 50.0f;
}

void AD1Character::InitAbilitySystem()
{
	if (AttributeSet)
	{
		AttributeSet->OnOutOfHealthDelegate.AddUObject(this, &ThisClass::HandleOutOfHealth);
	}
}

void AD1Character::ApplyAbilitySystemData(const FName& DataName)
{
	if (HasAuthority() == false)
		return;
	
	if (const UD1AbilitySystemData* Data = UD1AssetManager::GetAssetByName<UD1AbilitySystemData>(DataName))
	{
		Data->GiveToAbilitySystem(AbilitySystemComponent, &GrantedHandles);
	}
}

void AD1Character::StartDeath()
{
	if (HasAuthority())
	{
		Multicast_OnDeath();
	}

	GetWorldTimerManager().SetTimerForNextTick([this]()
	{
		if (AbilitySystemComponent)
		{
			if (AbilitySystemComponent->GetAvatarActor() == this)
			{
				AbilitySystemComponent->CancelAbilities();
				AbilitySystemComponent->ClearAbilityInput();
				AbilitySystemComponent->RemoveAllGameplayCues();

				if (AbilitySystemComponent->GetOwnerActor())
				{
					AbilitySystemComponent->SetAvatarActor(nullptr);
				}
				else
				{
					AbilitySystemComponent->ClearActorInfo();
				}
			
				AbilitySystemComponent = nullptr;
			}
		}
	});
}

void AD1Character::HandleOutOfHealth()
{
	if (HasAuthority())
	{
		FGameplayEventData Payload;
		AbilitySystemComponent->HandleGameplayEvent(D1GameplayTags::Ability_Death, &Payload);
	}
}

void AD1Character::Multicast_OnDeath_Implementation()
{
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	MovementComponent->StopMovementImmediately();
	MovementComponent->DisableMovement();
	
	UCapsuleComponent* CollisionComponent = GetCapsuleComponent();
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);

	PlayAnimMontage(UD1AssetManager::GetAssetByName<UAnimMontage>("DeathMontage"));
}

UAbilitySystemComponent* AD1Character::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
