#pragma once

#include "D1GameplayAbility_Weapon.h"
#include "D1GameplayAbility_Weapon_Spell.generated.h"

class AD1ProjectileBase;

UCLASS()
class UD1GameplayAbility_Weapon_Spell : public UD1GameplayAbility_Weapon
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Weapon_Spell(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	UFUNCTION(BlueprintCallable)
	void SpawnProjectile();

	UFUNCTION(BlueprintCallable)
	FGameplayAbilityTargetingLocationInfo GetStartLocation();
	
protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AD1ProjectileBase> ProjectileClass;

	UPROPERTY(EditDefaultsOnly)
	FName ProjectileSocketName;
};
