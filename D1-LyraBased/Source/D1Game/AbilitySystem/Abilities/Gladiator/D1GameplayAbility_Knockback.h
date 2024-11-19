#pragma once

#include "AbilitySystem/Abilities/LyraGameplayAbility.h"
#include "D1GameplayAbility_Knockback.generated.h"

UCLASS()
class UD1GameplayAbility_Knockback : public ULyraGameplayAbility
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Knockback(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
protected:
	UAnimMontage* SelectDirectionalMontage(const AActor* Source, const AActor* Target) const;

private:
	UFUNCTION()
	void OnKnockbackFinished();

	UFUNCTION()
	void ApplyStun();

protected:
	UPROPERTY(EditDefaultsOnly, Category="D1|Knockback")
	float KnockbackStrength = 1500.f;

	UPROPERTY(EditDefaultsOnly, Category="D1|Knockback")
	float KnockbackDuration = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category="D1|Knockback")
	TObjectPtr<UCurveFloat> KnockbackStrengthCurve;

	UPROPERTY(EditDefaultsOnly, Category="D1|Knockback")
	float StunApplyDelay = 0.3f;

	UPROPERTY(EditDefaultsOnly, Category="D1|Knockback")
	float ForwardThreshold = 50.f;

	UPROPERTY(EditDefaultsOnly, Category="D1|Knockback")
	float BackwardThreshold = 130.f;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="D1|Knockback")
	TObjectPtr<UAnimMontage> KnockbackForwardMontage;
	
	UPROPERTY(EditDefaultsOnly, Category="D1|Knockback")
	TObjectPtr<UAnimMontage> KnockbackBackwardMontage;

	UPROPERTY(EditDefaultsOnly, Category="D1|Knockback")
	TObjectPtr<UAnimMontage> KnockbackLeftMontage;

	UPROPERTY(EditDefaultsOnly, Category="D1|Knockback")
	TObjectPtr<UAnimMontage> KnockbackRightMontage;

private:
	float CachedStunDuration = 0.f;
};
