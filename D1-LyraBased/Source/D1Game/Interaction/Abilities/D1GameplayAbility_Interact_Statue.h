#pragma once

#include "D1GameplayAbility_Interact_Object.h"
#include "D1GameplayAbility_Interact_Statue.generated.h"

UCLASS()
class UD1GameplayAbility_Interact_Statue : public UD1GameplayAbility_Interact_Object
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Interact_Statue(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category="D1|Interaction")
	float HealthMagnitude = 60.f;
};
