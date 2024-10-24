#pragma once

#include "AbilitySystem/Abilities/LyraGameplayAbility.h"
#include "D1GameplayAbility_Death.generated.h"

struct FGameplayAbilityActorInfo;
struct FGameplayEventData;

UCLASS(Abstract)
class UD1GameplayAbility_Death : public ULyraGameplayAbility
{
	GENERATED_BODY()

public:
	UD1GameplayAbility_Death(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

public:
	UFUNCTION(BlueprintCallable)
	void StartDeath();
	
	UFUNCTION(BlueprintCallable)
	void FinishDeath();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "D1|Death")
	bool bAutoStartDeath;
};
