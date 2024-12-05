#pragma once

#include "AbilitySystem/Abilities/Gladiator/D1GameplayAbility_Equipment.h"
#include "D1GameplayAbility_Weapon_Block.generated.h"

UCLASS()
class UD1GameplayAbility_Weapon_Block : public UD1GameplayAbility_Equipment
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Weapon_Block(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	UFUNCTION()
	void OnInputRelease(float TimeHeld);
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="D1|Weapon Block")
	TObjectPtr<UAnimMontage> BlockStartMontage;

	UPROPERTY(EditDefaultsOnly, Category="D1|Weapon Block")
	TObjectPtr<UAnimMontage> BlockEndMontage;
};
