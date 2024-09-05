#pragma once

#include "D1GameplayAbility_Weapon.h"
#include "D1GameplayAbility_Weapon_GroundBreaker.generated.h"

UCLASS()
class UD1GameplayAbility_Weapon_GroundBreaker : public UD1GameplayAbility_Weapon
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Weapon_GroundBreaker(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
protected:
	UFUNCTION(BlueprintCallable)
	void Execute();

protected:
	UPROPERTY(EditDefaultsOnly)
	float Distance = 150.f;
	
private:
	UPROPERTY()
	TSet<TWeakObjectPtr<ALyraCharacter>> CachedHitCharacters;
};
