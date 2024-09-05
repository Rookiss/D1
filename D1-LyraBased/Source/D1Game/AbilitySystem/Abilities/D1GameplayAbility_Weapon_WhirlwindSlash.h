#pragma once

#include "D1GameplayAbility_Weapon.h"
#include "D1GameplayAbility_Weapon_WhirlwindSlash.generated.h"

UCLASS()
class UD1GameplayAbility_Weapon_WhirlwindSlash : public UD1GameplayAbility_Weapon
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Weapon_WhirlwindSlash(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
protected:
	UFUNCTION(BlueprintCallable)
	void OnTargetDataReady(const FGameplayAbilityTargetDataHandle& InTargetDataHandle);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Damage = 10.f;
	
private:
	UPROPERTY()
	TSet<TWeakObjectPtr<AActor>> HitActors;
};
