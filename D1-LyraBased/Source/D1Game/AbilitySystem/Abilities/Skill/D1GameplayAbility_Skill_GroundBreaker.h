#pragma once

#include "AbilitySystem/Abilities/Weapon/D1GameplayAbility_Weapon_Melee.h"
#include "D1GameplayAbility_Skill_GroundBreaker.generated.h"

UCLASS()
class UD1GameplayAbility_Skill_GroundBreaker : public UD1GameplayAbility_Weapon_Melee
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Skill_GroundBreaker(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
protected:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void Execute();

protected:
	UPROPERTY(EditDefaultsOnly)
	float DistanceOffset = 150.f;

	UPROPERTY(EditDefaultsOnly)
	float Damage = 20.f;
	
	UPROPERTY(EditDefaultsOnly)
	float StunDruation = 3.f;
};
