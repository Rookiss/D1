#pragma once

#include "AbilitySystem/Abilities/D1GameplayAbility.h"
#include "Interaction/D1Interactable.h"
#include "D1GameplayAbility_Interact.generated.h"

struct FD1InteractionInfo;

UCLASS()
class UD1GameplayAbility_Interact : public UD1GameplayAbility
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Interact(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UFUNCTION(BlueprintCallable)
	void HandleInteractionInfo(const FD1InteractionInfo& NewInteractionInfo);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void WaitInteractionPress();
	
	UFUNCTION(BlueprintCallable)
	void OnInteractionPressDetected();
	
	void TriggerInteraction();

private:
	void ShowInteractionPress(const FText& InteractionTitle, const FText& InteractionContent);
	void ShowInteractionProgress(float HoldTime);
	void HideInteractionWidget();
	
protected:
	UPROPERTY()
	FD1InteractionInfo TraceHitInfo;

	FTimerHandle HoldTimerHandle;
	
	UPROPERTY(EditDefaultsOnly)
	float InteractionScanRate = 0.1f;

	UPROPERTY(EditDefaultsOnly)
	float InteractionScanRange = 500.f;
};
