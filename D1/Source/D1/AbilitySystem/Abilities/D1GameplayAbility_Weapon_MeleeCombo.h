#pragma once

#include "D1GameplayAbility_Weapon.h"
#include "D1GameplayAbility_Weapon_MeleeCombo.generated.h"

UCLASS()
class UD1GameplayAbility_Weapon_MeleeCombo : public UD1GameplayAbility_Weapon
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Weapon_MeleeCombo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
protected:
	UFUNCTION(BlueprintCallable)
	void HandleTargetData(const FGameplayAbilityTargetDataHandle& InTargetDataHandle);
	
	UFUNCTION(BlueprintCallable)
	void TryContinueToNextStage();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UAnimMontage> BackwardMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag NextStageTag;

	UPROPERTY(EditDefaultsOnly)
	bool bShowDebug = false;
	
	UPROPERTY()
	TSet<TObjectPtr<AActor>> HitActors;
	
	UPROPERTY(BlueprintReadWrite)
	bool bInputReleased = false;
	
	bool bBlocked = false;
	bool bAttacked = false;
	FTimerHandle BlockMontageTimerHandle;
};
