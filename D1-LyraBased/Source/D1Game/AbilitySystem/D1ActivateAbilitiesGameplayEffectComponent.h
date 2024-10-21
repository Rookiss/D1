#pragma once

#include "GameplayEffectComponent.h"
#include "GameplayAbilitySpec.h"
#include "D1ActivateAbilitiesGameplayEffectComponent.generated.h"

UENUM()
enum class EGameplayEffectActivatedAbilityCancelPolicy : uint8
{
	DoNothing,
	CancelOnEffectEnd,
};

USTRUCT()
struct FGameplayAbilityActivateConfig
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayAbility> AbilityClass;
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag AbilityEventTag;

	UPROPERTY(EditDefaultsOnly)
	EGameplayEffectActivatedAbilityCancelPolicy CancelPolicy = EGameplayEffectActivatedAbilityCancelPolicy::DoNothing;
};

UCLASS(DisplayName="Activate Gameplay Abilities by Event")
class UD1ActivateAbilitiesGameplayEffectComponent : public UGameplayEffectComponent
{
	GENERATED_BODY()
	
public:
	UD1ActivateAbilitiesGameplayEffectComponent();

protected:
	virtual bool OnActiveGameplayEffectAdded(FActiveGameplayEffectsContainer& ActiveGEContainer, FActiveGameplayEffect& ActiveGE) const override;
	virtual void OnActiveGameplayEffectRemoved(const FGameplayEffectRemovalInfo& RemovalInfo, FActiveGameplayEffectsContainer* ActiveGEContainer) const;
	virtual void OnGameplayEffectApplied(FActiveGameplayEffectsContainer& ActiveGEContainer, FGameplayEffectSpec& GESpec, FPredictionKey& PredictionKey) const override;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif

protected:
	UPROPERTY(EditDefaultsOnly, Category="ActiveAbilities")
	TArray<FGameplayAbilityActivateConfig> ActivateAbilityConfigs;
};
