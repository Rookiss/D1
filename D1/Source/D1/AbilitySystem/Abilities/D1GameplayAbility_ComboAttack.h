#pragma once

#include "D1GameplayAbility.h"
#include "D1GameplayAbility_ComboAttack.generated.h"

UCLASS()
class UD1GameplayAbility_ComboAttack : public UD1GameplayAbility
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_ComboAttack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
private:
	int32 CurrentStage = 0;
	
	
};
