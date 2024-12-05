#pragma once

#include "AbilitySystem/Abilities/Gladiator/D1GameplayAbility_Equipment.h"
#include "D1GameplayAbility_Utility_Consume.generated.h"

UCLASS()
class UD1GameplayAbility_Utility_Consume : public UD1GameplayAbility_Equipment
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Utility_Consume(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	bool bItemUsed = false;
};
