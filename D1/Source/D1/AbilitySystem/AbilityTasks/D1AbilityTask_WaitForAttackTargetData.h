#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "D1AbilityTask_WaitForAttackTargetData.generated.h"

UCLASS()
class UD1AbilityTask_WaitForAttackTargetData : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, meta=(DisplayName="WaitFor"))
};
