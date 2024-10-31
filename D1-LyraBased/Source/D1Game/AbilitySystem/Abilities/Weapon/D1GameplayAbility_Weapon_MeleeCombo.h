#pragma once

#include "D1GameplayAbility_Weapon_MeleeAttack.h"
#include "D1GameplayAbility_Weapon_MeleeCombo.generated.h"

UCLASS()
class UD1GameplayAbility_Weapon_MeleeCombo : public UD1GameplayAbility_Weapon_MeleeAttack
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Weapon_MeleeCombo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void HandleMontageEvent(FGameplayEventData Payload) override;
	
private:
	void WaitInputContinue();
	void WaitInputStop();
	
private:
	UFUNCTION()
	void OnInputReleased(float TimeHeld);

	UFUNCTION()
	void OnInputStart();

	UFUNCTION()
	void OnInputCancel();
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="D1|Melee Combo")
	TSubclassOf<UD1GameplayAbility_Weapon_MeleeCombo> NextAbilityClass;

private:
	bool bInputPressed = false;
	bool bInputReleased = false;
};
