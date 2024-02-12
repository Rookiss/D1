#include "D1GameplayAbility_Interact_Active.h"

#include "AbilitySystemComponent.h"
#include "Interaction/D1Interactable.h"
#include "Player/D1PlayerController.h"
#include "UI/D1HUD.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Interact_Active)

UD1GameplayAbility_Interact_Active::UD1GameplayAbility_Interact_Active(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivationPolicy = ED1AbilityActivationPolicy::Manual;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UD1GameplayAbility_Interact_Active::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	HideInteractionWidget();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UD1GameplayAbility_Interact_Active::Initialize(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	CurrentTargetDataHandle = TargetDataHandle;
	TargetActor = TargetDataHandle.Get(0)->GetHitResult()->GetActor();
	
	if (ID1Interactable* Interactable = Cast<ID1Interactable>(TargetActor))
	{
		HoldTime = Interactable->GetInteractionInfo().HoldTime;
	}
}

void UD1GameplayAbility_Interact_Active::Interact()
{
	ID1Interactable::Execute_Interact(TargetActor.Get(), GetAvatarActorFromActorInfo());
}

void UD1GameplayAbility_Interact_Active::ShowInteractionProgressWidget()
{
	if (AD1PlayerController* PC = GetPlayerController())
	{
		if (AD1HUD* HUD = Cast<AD1HUD>(PC->GetHUD()))
		{
			HUD->ShowInteractionHasDurationWidget(HoldTime);
		}
	}
}

void UD1GameplayAbility_Interact_Active::HideInteractionWidget()
{
	if (AD1PlayerController* PC = GetPlayerController())
	{
		if (AD1HUD* HUD = Cast<AD1HUD>(PC->GetHUD()))
		{
			HUD->HideInteractionWidget();
		}
	}
}
