#pragma once

#include "D1GameplayAbility_Weapon.h"
#include "D1GameplayAbility_ComboAttack.generated.h"

UENUM(BlueprintType)
enum class EHitState : uint8
{
	None,
	Attacked,
	Blocked
};

UCLASS()
class UD1GameplayAbility_ComboAttack : public UD1GameplayAbility_Weapon
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_ComboAttack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	UFUNCTION(BlueprintCallable)
	void PerformHitDetection();
	
	UFUNCTION(BlueprintCallable)
	bool CheckInputPress();

	void OnTargetDataReady(const FGameplayAbilityTargetDataHandle& InData, FGameplayTag ApplicationTag);

	UFUNCTION()
	void ProcessTargetData(const FGameplayAbilityTargetDataHandle& InTargetData);

protected:
	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<UAnimMontage>> AttackMontages;

	bool bIsFirstTrace = true;
	FVector PrevWeaponLocation;
	FRotator PrevWeaponRotation;
	TSet<TObjectPtr<AActor>> HitActors;

	UPROPERTY(BlueprintReadWrite)
	int32 CurrentStage = 0;

	UPROPERTY(BlueprintReadWrite)
	bool bInputPressed = false;
	
	bool bAttacked = false;
	bool bBlocked = false;

	FDelegateHandle OnTargetDataReadyDelegateHandle;
};
