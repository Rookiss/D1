#pragma once

#include "D1GameplayAbility_Interact_Object.h"
#include "D1GameplayAbility_Interact_Pickup.generated.h"

UCLASS()
class UD1GameplayAbility_Interact_Pickup : public UD1GameplayAbility_Interact_Object
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Interact_Pickup(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
