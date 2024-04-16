#include "D1GameplayAbility.h"

#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/D1AbilitySystemComponent.h"
#include "Character/D1Character.h"
#include "Input/D1EnhancedPlayerInput.h"
#include "Player/D1PlayerController.h"
#include "UI/D1HUD.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility)

UD1GameplayAbility::UD1GameplayAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
	ActivationPolicy = ED1AbilityActivationPolicy::Manual;
}

void UD1GameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	TryActivateAbilityOnGiveOrSpawn(ActorInfo, Spec);
}

void UD1GameplayAbility::TryActivateAbilityOnGiveOrSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const
{
	const bool bIsPredicting = (Spec.ActivationInfo.ActivationMode == EGameplayAbilityActivationMode::Predicting);
	if (ActorInfo && Spec.IsActive() == false && bIsPredicting == false && ActivationPolicy == ED1AbilityActivationPolicy::OnGiveOrSpawn)
	{
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		const AActor* AvatarActor = ActorInfo->AvatarActor.Get();
		if (ASC && AvatarActor && AvatarActor->GetTearOff() == false && AvatarActor->GetLifeSpan() <= 0.f)
		{
			const bool bIsLocalExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalPredicted) || (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalOnly);
			const bool bIsServerExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerOnly) || (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerInitiated);

			const bool bClientShouldActivate = ActorInfo->IsLocallyControlled() && bIsLocalExecution;
			const bool bServerShouldActivate = ActorInfo->IsNetAuthority() && bIsServerExecution;
			if (bClientShouldActivate || bServerShouldActivate)
			{
				ASC->TryActivateAbility(Spec.Handle);
			}
		}
	}
}

void UD1GameplayAbility::AddInputMappingContext(UInputMappingContext* IMC)
{
	if (CurrentActorInfo && IMC)
	{
		if (APlayerController* PlayerController = CurrentActorInfo->PlayerController.Get())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				Subsystem->AddMappingContext(IMC, 0);
			}
		}
	}
}

void UD1GameplayAbility::RemoveInputMappingContext(UInputMappingContext* IMC)
{
	if (CurrentActorInfo && IMC)
	{
		if (APlayerController* PlayerController = CurrentActorInfo->PlayerController.Get())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				Subsystem->RemoveMappingContext(IMC);
			}
		}
	}
}

void UD1GameplayAbility::FlushPressedInput(UInputAction* InputAction)
{
	if (CurrentActorInfo)
	{
		if (APlayerController* PlayerController = CurrentActorInfo->PlayerController.Get())
		{
			if (UD1EnhancedPlayerInput* PlayerInput = Cast<UD1EnhancedPlayerInput>(PlayerController->PlayerInput))
			{
				PlayerInput->FlushPressedInput(InputAction);
			}
		}
	}
}

UD1AbilitySystemComponent* UD1GameplayAbility::GetAbilitySystemComponent() const
{
	return (CurrentActorInfo ? Cast<UD1AbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent.Get()) : nullptr);
}

AD1Character* UD1GameplayAbility::GetCharacter() const
{
	return (CurrentActorInfo ? Cast<AD1Character>(CurrentActorInfo->AvatarActor.Get()) : nullptr);
}

AD1PlayerController* UD1GameplayAbility::GetPlayerController() const
{
	return (CurrentActorInfo ? Cast<AD1PlayerController>(CurrentActorInfo->PlayerController.Get()) : nullptr);
}

AD1HUD* UD1GameplayAbility::GetHUD() const
{
	AD1HUD* HUD = nullptr;
	
	if (CurrentActorInfo)
	{
		if (AD1PlayerController* PC = GetPlayerController())
		{
			HUD = Cast<AD1HUD>(PC->GetHUD());
		}
	}

	return HUD;
}

bool UD1GameplayAbility::IsInputPressed() const
{
	FGameplayAbilitySpec* AbilitySpec = GetCurrentAbilitySpec();
	return (AbilitySpec && AbilitySpec->InputPressed);
}
