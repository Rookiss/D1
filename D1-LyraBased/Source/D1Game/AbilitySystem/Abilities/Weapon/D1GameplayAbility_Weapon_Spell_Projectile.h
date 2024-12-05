#pragma once

#include "AbilitySystem/Abilities/Gladiator/D1GameplayAbility_Equipment.h"
#include "D1GameplayAbility_Weapon_Spell_Projectile.generated.h"

class AD1ProjectileBase;
class UAbilityTask_WaitGameplayEvent;
class UAbilityTask_WaitConfirmCancel;

UCLASS()
class UD1GameplayAbility_Weapon_Spell_Projectile : public UD1GameplayAbility_Equipment
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Weapon_Spell_Projectile(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	void ConfirmSpell();
	void CancelSpell();
	void ResetSpell();
	
protected:
	UFUNCTION()
	void OnCastStartBegin(FGameplayEventData Payload);

	UFUNCTION()
	void OnSpellStart(FGameplayEventData Payload);
	
	UFUNCTION()
	void OnMontageFinished();

private:
	UFUNCTION()
	void OnInputConfirm();
	
	UFUNCTION()
	void OnInputCancel();

private:
	void OnPhaseTimePassed();
	void SpawnProjectile();
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="D1|Spell Projectile")
	UAnimMontage* CastStartMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="D1|Spell Projectile")
	UAnimMontage* CastEndMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="D1|Spell Projectile")
	UAnimMontage* SpellMontage = nullptr;

protected:
	UPROPERTY(EditDefaultsOnly, Category="D1|Spell Projectile", meta=(Categories="GameplayCue"))
	FGameplayTag CastGameplayCueTag;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="D1|Spell Projectile")
	TObjectPtr<UInputAction> MainHandInputAction;

	UPROPERTY(EditDefaultsOnly, Category="D1|Spell Projectile")
	TObjectPtr<UInputAction> OffHandInputAction;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="D1|Spell Projectile", EditFixedSize)
	TArray<TSubclassOf<AD1ProjectileBase>> Phase1_ProjectileClasses;
	
	UPROPERTY(EditDefaultsOnly, Category="D1|Spell Projectile", EditFixedSize)
	TArray<TSubclassOf<AD1ProjectileBase>> Phase2_ProjectileClasses;
	
	UPROPERTY(EditDefaultsOnly, Category="D1|Spell Projectile", EditFixedSize)
	TArray<TSubclassOf<AD1ProjectileBase>> Phase3_ProjectileClasses;

	UPROPERTY(EditDefaultsOnly, Category="D1|Spell Projectile", EditFixedSize)
	TArray<float> PhaseTimes;

	UPROPERTY(EditDefaultsOnly, Category="D1|Spell Projectile", EditFixedSize)
	TArray<FLinearColor> PhaseColors;
	
	UPROPERTY(EditDefaultsOnly, Category="D1|Spell Projectile")
	FName ProjectileSocketName;

	UPROPERTY(EditDefaultsOnly, Category="D1|Spell Projectile")
	float DeltaDegree = 5.0f;

private:
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> CastStartBeginEventTask;
	
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitConfirmCancel> InputConfirmTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitConfirmCancel> InputCancelTask;

private:
	int32 CurrentIndex = 0;
	float TotalCastTime = 0.f;
	FTimerHandle PhaseTimerHandle;
};
