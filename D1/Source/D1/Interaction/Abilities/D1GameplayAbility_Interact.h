#pragma once

#include "AbilitySystem/Abilities/D1GameplayAbility.h"
#include "D1GameplayAbility_Interact.generated.h"

class UD1GameplayAbility_Interact_Active;
struct FD1InteractionInfo;

UCLASS()
class UD1GameplayAbility_Interact : public UD1GameplayAbility
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Interact(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
protected:
	UFUNCTION(BlueprintCallable)
	void HandleInputPress();

protected:
	void UpdateWidget();
	
	void ShowInteractionPressWidget(const FD1InteractionInfo& InteractionInfo);
	void HideInteractionWidget();

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UD1GameplayAbility_Interact_Active> InteractActiveAbilityClass;
	
protected:
	UPROPERTY(EditDefaultsOnly)
	float InteractionScanRate = 0.1f;

	UPROPERTY(EditDefaultsOnly)
	float InteractionScanRange = 500.f;

protected:
	UPROPERTY(BlueprintReadWrite)
	FGameplayAbilityTargetDataHandle CurrentTargetDataHandle;
	
private:
	FTimerHandle UpdateWidgetTimerHandle;
};
