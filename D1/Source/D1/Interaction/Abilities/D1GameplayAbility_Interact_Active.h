#pragma once

#include "AbilitySystem/Abilities/D1GameplayAbility.h"
#include "D1GameplayAbility_Interact_Active.generated.h"

UCLASS()
class UD1GameplayAbility_Interact_Active : public UD1GameplayAbility
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Interact_Active(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
protected:
	UFUNCTION(BlueprintCallable)
	void Initialize(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
	
	UFUNCTION(BlueprintCallable)
	void Interact();

protected:
	UFUNCTION(BlueprintCallable)
	void ShowInteractionDurationWidget();

	UFUNCTION(BlueprintCallable)
	void HideInteractionDurationWidget();
	
protected:
	UPROPERTY(BlueprintReadOnly)
	FGameplayAbilityTargetDataHandle CurrentTargetDataHandle;
	
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<AActor> CachedTargetActor;

	UPROPERTY(BlueprintReadOnly)
	float CachedHoldTime = 0.f;
};
