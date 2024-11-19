#pragma once

#include "AbilitySystem/Abilities/LyraGameplayAbility.h"
#include "D1GameplayAbility_Sprint_Active.generated.h"

UCLASS()
class UD1GameplayAbility_Sprint_Active : public ULyraGameplayAbility
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Sprint_Active(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	UFUNCTION()
	void OnTick(float DeltaTime);

	UFUNCTION()
	void OnInputReleased(float TimeHeld);

	UFUNCTION()
	void OnSprintCommitTick();

protected:
	UPROPERTY(EditDefaultsOnly, Category="D1|Sprint Active")
	TSubclassOf<UGameplayEffect> RecoveryBlockEffectClass;

	UPROPERTY(EditDefaultsOnly, Category="D1|Sprint Active")
	float ForwardThreshold = 0.25f;

	UPROPERTY(EditDefaultsOnly, Category="D1|Sprint Active")
	float CommitInterval = 0.1f;
	
private:
	FTimerHandle SprintTimerHandle;
};
