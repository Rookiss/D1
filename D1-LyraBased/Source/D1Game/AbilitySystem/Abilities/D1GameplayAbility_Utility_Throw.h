#pragma once

#include "D1GameplayAbility_Weapon.h"
#include "D1GameplayAbility_Utility_Throw.generated.h"

UCLASS()
class UD1GameplayAbility_Utility_Throw : public UD1GameplayAbility_Weapon
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Utility_Throw(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
