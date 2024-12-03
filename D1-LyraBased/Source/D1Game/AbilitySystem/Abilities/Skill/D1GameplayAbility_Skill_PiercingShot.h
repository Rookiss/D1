#pragma once

#include "AbilitySystem/Abilities/Weapon/D1GameplayAbility_Weapon_Bow_Projectile.h"
#include "D1GameplayAbility_Skill_PiercingShot.generated.h"

class UAbilityTask_WaitGameplayEvent;
class UAbilityTask_WaitConfirmCancel;

UCLASS()
class UD1GameplayAbility_Skill_PiercingShot : public UD1GameplayAbility_Weapon_Bow_Projectile
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Skill_PiercingShot(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
protected:
	UFUNCTION()
	void OnPiercingShotBegin(FGameplayEventData Payload);

private:
	void ConfirmSkill();
	void CancelSkill();
	void ResetSkill();

private:
	UFUNCTION()
	void OnInputConfirm();
	
	UFUNCTION()
	void OnInputCancel();

	UFUNCTION()
	void OnMontageFinished();

protected:
	UPROPERTY(EditDefaultsOnly, Category="D1|Piercing Shot")
	TObjectPtr<UInputAction> MainHandInputAction;

	UPROPERTY(EditDefaultsOnly, Category="D1|Piercing Shot")
	TObjectPtr<UInputAction> OffHandInputAction;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="D1|Piercing Shot")
	TObjectPtr<UAnimMontage> ADSStartMontage;

	UPROPERTY(EditDefaultsOnly, Category="D1|Piercing Shot")
	TObjectPtr<UAnimMontage> ADSEndMontage;
	
	UPROPERTY(EditDefaultsOnly, Category="D1|Piercing Shot")
	TObjectPtr<UAnimMontage> ReleaseMontage;
	
	UPROPERTY(EditDefaultsOnly, Category="D1|Piercing Shot")
	TSubclassOf<ULyraCameraMode> ADSCameraModeClass;

private:
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> PiercingShotBeginEventTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitConfirmCancel> SkillConfirmTask;
	
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitConfirmCancel> SkillCancelTask;
};
