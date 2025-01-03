#pragma once

#include "AbilitySystem/Abilities/Gladiator/D1GameplayAbility_Equipment.h"
#include "D1GameplayAbility_Bow_AutoReload.generated.h"

class UD1ItemTemplate;

UCLASS()
class UD1GameplayAbility_Bow_AutoReload : public UD1GameplayAbility_Equipment
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Bow_AutoReload(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
private:
	void PeriodicCheck();
	bool CheckAmmoState();

protected:
	UPROPERTY(EditDefaultsOnly, Category="D1|Auto Reload")
	float CheckInterval = 0.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="D1|Auto Reload")
	FGameplayTagContainer CheckTagContainer;
	
private:
	FTimerHandle TimerHandle;
	
	UPROPERTY()
	TWeakObjectPtr<UD1ItemInstance> WeaponItemInstance;
	
	UPROPERTY()
	TSubclassOf<UD1ItemTemplate> AmmoItemTemplateClass;
};
