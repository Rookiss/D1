#pragma once

#include "AbilitySystem/Abilities/D1GameplayAbility.h"
#include "D1GamePhaseAbility.generated.h"

UCLASS(Abstract, HideCategories=Input)
class UD1GamePhaseAbility : public UD1GameplayAbility
{
	GENERATED_BODY()

public:
	UD1GamePhaseAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

public:
	const FGameplayTag& GetGamePhaseTag() const { return GamePhaseTag; }
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="D1|Game Phase")
	FGameplayTag GamePhaseTag;
};
