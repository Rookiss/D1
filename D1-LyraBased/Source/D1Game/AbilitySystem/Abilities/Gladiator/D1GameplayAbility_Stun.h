#pragma once

#include "AbilitySystem/Abilities/LyraGameplayAbility.h"
#include "D1GameplayAbility_Stun.generated.h"

class UAnimMontage;

UCLASS()
class UD1GameplayAbility_Stun : public ULyraGameplayAbility
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Stun(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category="D1|Stun")
	TObjectPtr<UAnimMontage> StunMontage;
};
