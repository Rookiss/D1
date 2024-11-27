#pragma once

#include "D1GameplayAbility_Interact_Object.h"
#include "D1GameplayAbility_Interact_Door.generated.h"

UCLASS()
class UD1GameplayAbility_Interact_Door : public UD1GameplayAbility_Interact_Object
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Interact_Door(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
