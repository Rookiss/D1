#include "D1GameplayAbility_Weapon_Bow_ADS.h"

#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Weapon_Bow_ADS)

UD1GameplayAbility_Weapon_Bow_ADS::UD1GameplayAbility_Weapon_Bow_ADS(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1GameplayAbility_Weapon_Bow_ADS::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	SetCameraMode(CameraMode);

	WaitInputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this, true);
	WaitInputReleaseTask->OnRelease.AddDynamic(this, &ThisClass::OnInputRelease);
	WaitInputReleaseTask->ReadyForActivation();
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UD1GameplayAbility_Weapon_Bow_ADS::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (IsValid(WaitInputReleaseTask))
	{
		WaitInputReleaseTask->EndTask();
	}
	
	ClearCameraMode();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UD1GameplayAbility_Weapon_Bow_ADS::OnInputRelease(float TimeHeld)
{
	K2_EndAbility();
}
