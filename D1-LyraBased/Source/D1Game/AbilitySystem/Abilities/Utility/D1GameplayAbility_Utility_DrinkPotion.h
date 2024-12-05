#pragma once

#include "D1GameplayAbility_Utility_Consume.h"
#include "D1GameplayAbility_Utility_DrinkPotion.generated.h"

UCLASS()
class UD1GameplayAbility_Utility_DrinkPotion : public UD1GameplayAbility_Utility_Consume
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Utility_DrinkPotion(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	UFUNCTION()
	void OnDrinkBegin(FGameplayEventData Payload);
	
	UFUNCTION()
	void OnMontageFinished();

private:
	void ApplyUtilityGameplayEffect();
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="D1|Utility Drink")
	TObjectPtr<UAnimMontage> DrinkMontage;
};
