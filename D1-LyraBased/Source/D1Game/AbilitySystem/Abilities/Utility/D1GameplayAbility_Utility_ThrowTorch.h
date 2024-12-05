#pragma once

#include "D1GameplayAbility_Utility_Consume.h"
#include "D1GameplayAbility_Utility_ThrowTorch.generated.h"

class AD1PickupableItemBase;
class UAbilityTask_WaitInputRelease;

UCLASS()
class UD1GameplayAbility_Utility_ThrowTorch : public UD1GameplayAbility_Utility_Consume
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Utility_ThrowTorch(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	UFUNCTION()
	void OnInputRelease(float TimeHeld);
	
	UFUNCTION()
	void OnInputConfirm();

private:
	UFUNCTION()
	void OnThrowFinished();

	UFUNCTION()
	void OnThrowEndFinished();

private:
	UFUNCTION()
	void OnThrowEvent(FGameplayEventData Payload);

	UFUNCTION()
	void OnNetSync();
	
private:
	FTransform GetSpawnTransform();
	void SpawnThrowableItem();

protected:
	UPROPERTY(EditDefaultsOnly, Category="D1|Utility Throw")
	TObjectPtr<UAnimMontage> ThrowStartMontage;

	UPROPERTY(EditDefaultsOnly, Category="D1|Utility Throw")
	TObjectPtr<UAnimMontage> ThrowMontage;

	UPROPERTY(EditDefaultsOnly, Category="D1|Utility Throw")
	TObjectPtr<UAnimMontage> ThrowEndMontage;
	
	UPROPERTY(EditDefaultsOnly, Category="D1|Utility Throw")
	TSubclassOf<AD1PickupableItemBase> ThrowableItemClass;

private:
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitInputRelease> InputReleaseTask;
};
