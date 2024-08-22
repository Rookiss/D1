#pragma once

#include "D1GameplayAbility_Weapon.h"
#include "D1GameplayAbility_Utility_Drink.generated.h"

UCLASS()
class UD1GameplayAbility_Utility_Drink : public UD1GameplayAbility_Weapon
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Utility_Drink(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UFUNCTION(BlueprintCallable)
	void ApplyUtilityGameplayEffect();
};
