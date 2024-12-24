#pragma once

#include "AbilitySystem/Abilities/Gladiator/D1GameplayAbility_Equipment.h"
#include "D1GameplayAbility_Bow_Reload.generated.h"

UCLASS()
class UD1GameplayAbility_Bow_Reload : public UD1GameplayAbility_Equipment
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Bow_Reload(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	UPROPERTY(EditDefaultsOnly, Category="D1|Bow Reload")
	TObjectPtr<UAnimMontage> ReloadMontage;
};
