#pragma once

#include "AbilitySystem/Abilities/D1GameplayAbility.h"
#include "Interaction/D1InteractionInfo.h"
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
	void RefreshInteractionWidget(const FD1InteractionInfo& InteractionInfo);
	
	UFUNCTION(BlueprintCallable)
	void TriggerInteraction();

public:
	UFUNCTION(BlueprintCallable)
	bool IsInfoValid() const { return LatestInfo.Interactable != nullptr; }
	
protected:
	UPROPERTY(BlueprintReadOnly)
	FD1InteractionInfo LatestInfo;
	
	UPROPERTY(EditDefaultsOnly)
	float InteractionScanRate = 0.1f;

	UPROPERTY(EditDefaultsOnly)
	float InteractionScanRange = 500.f;
};
