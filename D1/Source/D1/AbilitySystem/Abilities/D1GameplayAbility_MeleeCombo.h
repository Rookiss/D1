#pragma once

#include "D1GameplayAbility_Weapon.h"
#include "D1GameplayAbility_MeleeCombo.generated.h"

UCLASS()
class UD1GameplayAbility_MeleeCombo : public UD1GameplayAbility_Weapon
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_MeleeCombo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

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
	
	UPROPERTY()
	TSet<TObjectPtr<AActor>> HitActors;
	
	UPROPERTY(BlueprintReadWrite)
	bool bInputReleased = false;
	
	bool bBlocked = false;
	FTimerHandle BlockMontageTimerHandle;
};
