#pragma once

#include "Abilities/GameplayAbilityWorldReticle.h"
#include "AbilitySystem/Abilities/Weapon/D1GameplayAbility_Weapon.h"
#include "D1GameplayAbility_Skill_AOE.generated.h"

class AGameplayAbilityTargetActor_GroundTrace;
class UAbilityTask_WaitConfirmCancel;
class UAbilityTask_WaitGameplayEvent;

UCLASS()
class UD1GameplayAbility_Skill_AOE : public UD1GameplayAbility_Weapon
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Skill_AOE(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UFUNCTION(BlueprintCallable)
	void ConfirmSkill();

	UFUNCTION(BlueprintCallable)
	void CancelSkill();

	UFUNCTION(BlueprintCallable)
	void ResetSkill();
	
	UFUNCTION(BlueprintImplementableEvent)
	void WaitTargetData();
	
private:
	UFUNCTION()
	void OnCastStartMontageBegin(FGameplayEventData Payload);

	UFUNCTION()
	void OnSpellMontageBegin(FGameplayEventData Payload);
	
	UFUNCTION()
	void OnSpellMontageEnd(FGameplayEventData Payload);

	UFUNCTION()
	void OnMontageFinished();

private:
	UFUNCTION()
	void OnInputConfirm();
	
	UFUNCTION()
	void OnInputCancel();

private:
	UFUNCTION()
	void OnValidData(const FGameplayAbilityTargetDataHandle& Data);

	UFUNCTION()
	void OnCancelled(const FGameplayAbilityTargetDataHandle& Data);
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="D1|AOE")
	UAnimMontage* CastStartMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="D1|AOE")
	UAnimMontage* CastEndMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="D1|AOE")
	UAnimMontage* SpellMontage = nullptr;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="D1|AOE", meta=(Categories="GameplayCue"))
	FGameplayTag CastGameplayCueTag;
	
	UPROPERTY(EditDefaultsOnly, Category="D1|AOE", DisplayName="AOE Spawner Class")
	TSubclassOf<AActor> AOESpawnerClass;

protected:
	UPROPERTY(EditDefaultsOnly, Category="D1|AOE")
	TObjectPtr<UInputAction> MainHandInputAction;

	UPROPERTY(EditDefaultsOnly, Category="D1|AOE")
	TObjectPtr<UInputAction> OffHandInputAction;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="D1|AOE")
	TSubclassOf<AGameplayAbilityTargetActor_GroundTrace> TargetActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="D1|AOE", DisplayName="AOE Reticle Class")
	TSubclassOf<AGameplayAbilityWorldReticle> AOEReticleClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="D1|AOE")
	float CollisionRadius = 3.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="D1|AOE")
	float CollisionHeight = 3.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="D1|AOE")
	float MaxRange = 1000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="D1|AOE")
	FCollisionProfileName TraceProfile;

private:
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> CastStartMontageBeginEventTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitConfirmCancel> SkillConfirmCancelTask;

	UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	FGameplayAbilityTargetDataHandle TargetDataHandle;
};
