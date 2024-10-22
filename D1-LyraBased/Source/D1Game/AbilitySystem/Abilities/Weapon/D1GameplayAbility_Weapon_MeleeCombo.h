#pragma once

#include "D1GameplayAbility_Weapon_Melee.h"
#include "D1GameplayAbility_Weapon_MeleeCombo.generated.h"

UCLASS()
class UD1GameplayAbility_Weapon_MeleeCombo : public UD1GameplayAbility_Weapon_Melee
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Weapon_MeleeCombo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

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

private:
	UFUNCTION()
	void OnTargetDataReady(FGameplayEventData Payload);

	UFUNCTION()
	void OnMontageFinished();
	
	UFUNCTION()
	void TryContinueToNextStage(FGameplayEventData Payload);
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> BackwardMontage;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UD1GameplayAbility_Weapon_MeleeCombo> NextAbilityClass;

private:
	bool bInputPressed = false;
	bool bInputReleased = false;

private:
	bool bBlocked = false;
	FTimerHandle BlockMontageTimerHandle;
};
