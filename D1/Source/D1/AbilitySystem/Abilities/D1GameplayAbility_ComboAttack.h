#pragma once

#include "D1GameplayAbility_Weapon.h"
#include "D1GameplayAbility_ComboAttack.generated.h"

UCLASS()
class UD1GameplayAbility_ComboAttack : public UD1GameplayAbility_Weapon
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_ComboAttack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UFUNCTION(BlueprintCallable)
	void HandleTargetData(const FGameplayAbilityTargetDataHandle& InTargetDataHandle);
	
	UFUNCTION(BlueprintCallable)
	bool CanContinueToNextStage();

	void OnTargetDataReady(const FGameplayAbilityTargetDataHandle& InTargetData, FGameplayTag ApplicationTag);

	UFUNCTION()
	void ProcessTargetData(const FGameplayAbilityTargetDataHandle& InTargetData);

private:
	void HandleBlockMontage();

protected:
	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<UAnimMontage>> AttackMontages;

	UPROPERTY()
	TSet<TObjectPtr<AActor>> HitActors;

	UPROPERTY(BlueprintReadWrite)
	int32 CurrentStage = 0;

	UPROPERTY(BlueprintReadWrite)
	bool bInputPressed = false;
	
	bool bBlocked = false;
	FDelegateHandle OnTargetDataReadyDelegateHandle;
	FTimerHandle BlockMontageTimerHandle;
};
