#pragma once

#include "D1GameplayAbility_Weapon_Range.h"
#include "D1GameplayAbility_Weapon_Bow_Shoot.generated.h"

UCLASS()
class UD1GameplayAbility_Weapon_Bow_Shoot : public UD1GameplayAbility_Weapon_Range
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Weapon_Bow_Shoot(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
protected:
	UFUNCTION()
	void OnMontageFinished();
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="D1|Bow Shoot")
	TObjectPtr<UAnimMontage> ReleaseMontage;
	
	UPROPERTY(EditDefaultsOnly, Category="D1|Bow Shoot")
	TObjectPtr<UAnimMontage> ReloadMontage;
	
	UPROPERTY(EditDefaultsOnly, Category="D1|Bow Shoot")
	TObjectPtr<UAnimMontage> ReleaseReloadMontage;
};
