#pragma once

#include "D1GameplayAbility_Weapon.h"
#include "D1GameplayAbility_Weapon_Spell.generated.h"

UCLASS()
class UD1GameplayAbility_Weapon_Spell : public UD1GameplayAbility_Weapon
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Weapon_Spell(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
