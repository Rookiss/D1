#pragma once

#include "AbilitySystem/Abilities/Gladiator/D1GameplayAbility_Equipment.h"
#include "D1GameplayAbility_Weapon_Bow_Projectile.generated.h"

class AD1ProjectileBase;

UCLASS()
class UD1GameplayAbility_Weapon_Bow_Projectile : public UD1GameplayAbility_Equipment
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Weapon_Bow_Projectile(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	void SpawnProjectile();

protected:
	UPROPERTY(EditDefaultsOnly, Category="D1|Bow Projectile")
	TSubclassOf<AD1ProjectileBase> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category="D1|Bow Projectile")
	FName SpawnSocketName;

	UPROPERTY(EditDefaultsOnly, Category="D1|Bow Projectile")
	bool bApplyAimAssist = true;
	
	UPROPERTY(EditDefaultsOnly, Category="D1|Bow Projectile", meta=(EditCondition="bApplyAnimAssist"))
	float AimAssistMinDistance = 100.f;
	
	UPROPERTY(EditDefaultsOnly, Category="D1|Bow Projectile", meta=(EditCondition="bApplyAnimAssist"))
	float AimAssistMaxDistance = 10000.f;
};
