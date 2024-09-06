#pragma once

#include "D1GameplayAbility_Weapon_Melee.h"
#include "D1GameplayAbility_Weapon_WhirlwindSlash.generated.h"

UCLASS()
class UD1GameplayAbility_Weapon_WhirlwindSlash : public UD1GameplayAbility_Weapon_Melee
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Weapon_WhirlwindSlash(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	UFUNCTION(BlueprintCallable)
	void OnTargetDataReady(const FGameplayAbilityTargetDataHandle& InTargetDataHandle);
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Damage = 10.f;
};
