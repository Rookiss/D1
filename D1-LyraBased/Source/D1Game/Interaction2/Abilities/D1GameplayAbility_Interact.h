#pragma once

#include "AbilitySystem/Abilities/LyraGameplayAbility.h"
#include "D1GameplayAbility_Interact.generated.h"

class UIndicatorDescriptor;
class UD1GameplayAbility_Interact_Active;
struct FD1InteractionInfo;

UCLASS()
class UD1GameplayAbility_Interact : public ULyraGameplayAbility
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Interact(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UFUNCTION(BlueprintCallable)
	void UpdateInteractions(const TArray<FD1InteractionInfo>& InteractionInfos);

	UFUNCTION(BlueprintCallable)
	void TriggerInteraction();

protected:
	UPROPERTY(BlueprintReadWrite)
	TArray<FD1InteractionInfo> CurrentInteractionInfos;
	
	UPROPERTY()
	TArray<TObjectPtr<UIndicatorDescriptor>> Indicators;
	
protected:
	UPROPERTY(EditDefaultsOnly)
	float InteractionScanRate = 0.1f;

	UPROPERTY(EditDefaultsOnly)
	float InteractionScanRange = 500.f;
	
	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UUserWidget> DefaultInteractionWidgetClass;
};
