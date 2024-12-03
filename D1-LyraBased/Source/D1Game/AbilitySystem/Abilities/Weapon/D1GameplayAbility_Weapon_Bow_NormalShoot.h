#pragma once

#include "D1GameplayAbility_Weapon_Bow_Projectile.h"
#include "D1GameplayAbility_Weapon_Bow_NormalShoot.generated.h"

UCLASS()
class UD1GameplayAbility_Weapon_Bow_NormalShoot : public UD1GameplayAbility_Weapon_Bow_Projectile
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Weapon_Bow_NormalShoot(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
protected:
	UFUNCTION()
	void OnMontageFinished();
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="D1|Bow NormalShoot")
	TObjectPtr<UAnimMontage> ReleaseMontage;
	
	UPROPERTY(EditDefaultsOnly, Category="D1|Bow NormalShoot")
	TObjectPtr<UAnimMontage> ReloadMontage;
	
	UPROPERTY(EditDefaultsOnly, Category="D1|Bow NormalShoot")
	TObjectPtr<UAnimMontage> ReleaseReloadMontage;
};
