#pragma once

#include "D1GameplayAbility_Weapon.h"
#include "D1GameplayAbility_Weapon_Bow_Shoot.generated.h"

class AD1ProjectileBase;
class UAbilityTask_PlayMontageAndWait;

UCLASS()
class UD1GameplayAbility_Weapon_Bow_Shoot : public UD1GameplayAbility_Weapon
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Weapon_Bow_Shoot(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
private:
	UFUNCTION()
	void OnMontageFinished();
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="Asset")
	TObjectPtr<UAnimMontage> ReleaseMontage;

	UPROPERTY(EditDefaultsOnly, Category="Asset")
	TSubclassOf<AD1ProjectileBase> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category="Asset")
	FName SpawnSocketName;

	UPROPERTY(EditDefaultsOnly, Category="Asset")
	float AimAssistMinDistance = 100.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Asset")
	float AimAssistMaxDistance = 10000.f;
	
private:
	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> ReleaseMontageTask;
};
