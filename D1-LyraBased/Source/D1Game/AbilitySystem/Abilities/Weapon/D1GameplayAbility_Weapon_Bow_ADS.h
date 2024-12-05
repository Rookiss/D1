#pragma once

#include "AbilitySystem/Abilities/Gladiator/D1GameplayAbility_Equipment.h"
#include "D1GameplayAbility_Weapon_Bow_ADS.generated.h"

class ULyraCameraMode;
class UAbilityTask_WaitInputRelease;
class UAbilityTask_WaitGameplayEvent;

UCLASS()
class UD1GameplayAbility_Weapon_Bow_ADS : public UD1GameplayAbility_Equipment
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Weapon_Bow_ADS(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UFUNCTION()
	void OnADSEvent(FGameplayEventData Payload);

	UFUNCTION()
	void OnADSStartBegin(FGameplayEventData Payload);
	
	UFUNCTION()
	void OnInputRelease(float TimeHeld);

	UFUNCTION()
	void OnAttackEnd();

private:
	void StartADS();
	void ResetADS();
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="D1|Bow ADS")
	TObjectPtr<UAnimMontage> ADSStartMontage;

	UPROPERTY(EditDefaultsOnly, Category="D1|Bow ADS")
	TObjectPtr<UAnimMontage> ADSEndMontage;
	
private:
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> ADSEventTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> ADSStartBeginEventTask;
	
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitInputRelease> InputReleaseTask;
};
