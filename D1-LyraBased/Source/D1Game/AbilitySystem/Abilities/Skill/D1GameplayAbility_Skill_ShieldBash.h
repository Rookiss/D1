#pragma once

#include "AbilitySystem/Abilities/Gladiator/D1GameplayAbility_Equipment.h"
#include "D1GameplayAbility_Skill_ShieldBash.generated.h"

UCLASS()
class UD1GameplayAbility_Skill_ShieldBash : public UD1GameplayAbility_Equipment
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Skill_ShieldBash(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
private:
	UFUNCTION()
	void OnShieldBashBegin(FGameplayEventData Payload);
	
	UFUNCTION()
	void OnMontageFinished();
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="D1|Shield Bash")
	TObjectPtr<UAnimMontage> ShieldBashMontage;

	UPROPERTY(EditDefaultsOnly, Category="D1|Shield Bash")
	float Damage = 50.f;

	UPROPERTY(EditDefaultsOnly, Category="D1|Shield Bash")
	float StunDuration = 1.5f;
	
	UPROPERTY(EditDefaultsOnly, Category="D1|Shield Bash")
	float Distance = 100.f;

	UPROPERTY(EditDefaultsOnly, Category="D1|Shield Bash")
	float RadiusMultiplier = 3.25f;
};
