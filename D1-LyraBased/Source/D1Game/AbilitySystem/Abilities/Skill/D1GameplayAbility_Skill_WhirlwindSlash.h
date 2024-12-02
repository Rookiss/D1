#pragma once

#include "AbilitySystem/Abilities/Weapon/D1GameplayAbility_Weapon_Melee.h"
#include "D1GameplayAbility_Skill_WhirlwindSlash.generated.h"

UCLASS()
class UD1GameplayAbility_Skill_WhirlwindSlash : public UD1GameplayAbility_Weapon_Melee
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Skill_WhirlwindSlash(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UFUNCTION()
	void OnTrace(FGameplayEventData Payload);

	UFUNCTION()
	void OnReset(FGameplayEventData Payload);

	UFUNCTION()
	void OnWhirlwindSlashBegin(FGameplayEventData Payload);
	
	UFUNCTION()
	void OnWhirlwindSlashEnd(FGameplayEventData Payload);
	
	UFUNCTION()
	void OnMontageFinished();
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="D1|Whirlwind Slash")
	TObjectPtr<UAnimMontage> WhirlwindSlashMontage;
	
	UPROPERTY(EditDefaultsOnly, Category="D1|Whirlwind Slash")
	float Damage = 10.f;
};
