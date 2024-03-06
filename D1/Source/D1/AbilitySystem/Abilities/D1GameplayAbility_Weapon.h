#pragma once

#include "D1GameplayAbility.h"
#include "D1GameplayAbility_Weapon.generated.h"

class AD1WeaponBase;

UCLASS()
class UD1GameplayAbility_Weapon : public UD1GameplayAbility
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Weapon(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<AD1WeaponBase> WeaponActor;
};
