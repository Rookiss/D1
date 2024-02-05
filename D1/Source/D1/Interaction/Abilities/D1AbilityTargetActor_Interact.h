#pragma once

#include "Abilities/GameplayAbilityTargetActor_Trace.h"
#include "D1AbilityTargetActor_Interact.generated.h"

UCLASS(Blueprintable)
class AD1AbilityTargetActor_Interact : public AGameplayAbilityTargetActor_Trace
{
	GENERATED_BODY()
	
public:
	AD1AbilityTargetActor_Interact(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
