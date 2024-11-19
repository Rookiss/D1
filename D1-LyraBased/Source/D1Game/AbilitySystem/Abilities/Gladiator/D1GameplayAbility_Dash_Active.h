#pragma once

#include "AbilitySystem/Abilities/LyraGameplayAbility.h"
#include "D1GameplayAbility_Dash_Active.generated.h"

class UAnimMontage;

UCLASS()
class UD1GameplayAbility_Dash_Active : public ULyraGameplayAbility
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Dash_Active(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
protected:
	UFUNCTION()
	UAnimMontage* SelectDirectionalMontage(ED1Direction MovementDirection) const;

private:
	UFUNCTION()
	void OnMontageFinished();

	UFUNCTION()
	void OnDashFinished();
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="D1|Dash Active")
	float DashStrength = 450.f;

	UPROPERTY(EditDefaultsOnly, Category="D1|Dash Active")
	float DashDuration = 0.5f;
	
	UPROPERTY(EditDefaultsOnly, Category="D1|Dash Active")
	TObjectPtr<UAnimMontage> BackwardMontage;

	UPROPERTY(EditDefaultsOnly, Category="D1|Dash Active")
	TObjectPtr<UAnimMontage> LeftMontage;

	UPROPERTY(EditDefaultsOnly, Category="D1|Dash Active")
	TObjectPtr<UAnimMontage> RightMontage;
};
