#pragma once

#include "AbilitySystem/Abilities/LyraGameplayAbility.h"
#include "D1GameplayAbility_Bow_Reload.generated.h"

UCLASS()
class UD1GameplayAbility_Bow_Reload : public ULyraGameplayAbility
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Bow_Reload(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	bool IsBowLoaded() const;
};
