#pragma once

#include "D1GameplayAbility_Bow_Projectile.h"
#include "D1GameplayAbility_Bow_NormalShoot.generated.h"

UCLASS()
class UD1GameplayAbility_Bow_NormalShoot : public UD1GameplayAbility_Bow_Projectile
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Bow_NormalShoot(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
protected:
	UFUNCTION()
	void OnMontageFinished();

private:
	void NotifyToADS(bool bShouldCancel);
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="D1|Bow NormalShoot")
	TObjectPtr<UAnimMontage> ReleaseMontage;
};
