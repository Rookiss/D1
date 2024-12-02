#pragma once

#include "D1GameplayAbility_Weapon.h"
#include "D1GameplayAbility_Weapon_Range.generated.h"

class AD1ProjectileBase;

UCLASS()
class UD1GameplayAbility_Weapon_Range : public UD1GameplayAbility_Weapon
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Weapon_Range(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	void SpawnProjectileWithAssist();

protected:
	UPROPERTY(EditDefaultsOnly, Category="D1|Weapon Range")
	TSubclassOf<AD1ProjectileBase> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category="D1|Weapon Range")
	FName SpawnSocketName;

	UPROPERTY(EditDefaultsOnly, Category="D1|Weapon Range")
	bool bApplyAimAssist = true;
	
	UPROPERTY(EditDefaultsOnly, Category="D1|Weapon Range", meta=(EditCondition="bApplyAnimAssist"))
	float AimAssistMinDistance = 100.f;
	
	UPROPERTY(EditDefaultsOnly, Category="D1|Weapon Range", meta=(EditCondition="bApplyAnimAssist"))
	float AimAssistMaxDistance = 10000.f;
};
