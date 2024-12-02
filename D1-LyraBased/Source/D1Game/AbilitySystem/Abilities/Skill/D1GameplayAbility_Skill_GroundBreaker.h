#pragma once

#include "AbilitySystem/Abilities/Weapon/D1GameplayAbility_Weapon_Melee.h"
#include "D1GameplayAbility_Skill_GroundBreaker.generated.h"

UCLASS()
class UD1GameplayAbility_Skill_GroundBreaker : public UD1GameplayAbility_Weapon_Melee
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Skill_GroundBreaker(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
protected:
	UFUNCTION()
	void OnGroundBreakerBegin(FGameplayEventData Payload);

	UFUNCTION()
	void OnMontageFinished();

private:
	void ExecuteGroundBreaker();
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="D1|Ground Breaker")
	TObjectPtr<UAnimMontage> GroundBreakerMontage;
	
	UPROPERTY(EditDefaultsOnly, Category="D1|Ground Breaker")
	float DistanceOffset = 150.f;

	UPROPERTY(EditDefaultsOnly, Category="D1|Ground Breaker")
	float Damage = 80.f;
	
	UPROPERTY(EditDefaultsOnly, Category="D1|Ground Breaker")
	float StunDruation = 3.f;
};
