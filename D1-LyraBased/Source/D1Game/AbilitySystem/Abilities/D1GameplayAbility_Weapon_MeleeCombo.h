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
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
protected:
	UFUNCTION(BlueprintCallable)
	void OnTargetDataReady(const FGameplayAbilityTargetDataHandle& InTargetDataHandle);
	
	UFUNCTION(BlueprintCallable)
	void TryContinueToNextStage();

private:
	void DrawDebugHitPoint(const FHitResult& HitResult);
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UAnimMontage> BackwardMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UD1GameplayAbility_Weapon_MeleeCombo> NextAbilityClass;
	
	UPROPERTY(EditDefaultsOnly)
	bool bShowDebug = false;
	
protected:
	UPROPERTY(BlueprintReadWrite)
	bool bInputPressed = false;

	UPROPERTY(BlueprintReadWrite)
	bool bInputReleased = false;
	
private:
	UPROPERTY()
	TSet<TWeakObjectPtr<AActor>> HitActors;
	
	bool bBlocked = false;
	FTimerHandle BlockMontageTimerHandle;
};
