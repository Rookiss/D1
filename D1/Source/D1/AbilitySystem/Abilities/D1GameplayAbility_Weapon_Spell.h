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
	void ShowCastTrailEffect(const FVector& Location);
	
protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AD1ProjectileBase> ProjectileClass;

	UPROPERTY(EditDefaultsOnly)
	FName ProjectileSocketName;

	UPROPERTY(EditDefaultsOnly)
	float BaseDamage = 0.f;
};
