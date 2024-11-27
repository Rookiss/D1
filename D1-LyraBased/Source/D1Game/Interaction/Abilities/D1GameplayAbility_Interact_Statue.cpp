#include "D1GameplayAbility_Interact_Statue.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "D1GameplayTags.h"
#include "System/LyraAssetManager.h"
#include "System/LyraGameData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Interact_Statue)

UD1GameplayAbility_Interact_Statue::UD1GameplayAbility_Interact_Statue(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1GameplayAbility_Interact_Statue::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (TriggerEventData == nullptr || bInitialized == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	FGameplayCueParameters Parameters;
	Parameters.Instigator = InteractableActor;
	K2_ExecuteGameplayCueWithParams(D1GameplayTags::GameplayCue_Statue_Success, Parameters);

	if (HasAuthority(&CurrentActivationInfo))
	{
		TSubclassOf<UGameplayEffect> HealGE = ULyraAssetManager::GetSubclassByPath(ULyraGameData::Get().HealGameplayEffect_SetByCaller);
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(HealGE);
		if (FGameplayEffectSpec* EffectSpec = EffectSpecHandle.Data.Get())
		{
			EffectSpec->SetSetByCallerMagnitude(D1GameplayTags::SetByCaller_BaseHealth, HealthMagnitude);
			ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle);
		}
	}
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
