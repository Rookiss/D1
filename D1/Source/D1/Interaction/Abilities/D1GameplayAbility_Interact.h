#pragma once

#include "AbilitySystem/Abilities/D1GameplayAbility.h"
#include "D1GameplayAbility_Interact.generated.h"

UCLASS()
class UD1GameplayAbility_Interact : public UD1GameplayAbility
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Interact(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
