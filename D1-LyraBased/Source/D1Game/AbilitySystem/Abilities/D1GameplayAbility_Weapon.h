#pragma once

#include "D1Define.h"
#include "LyraGameplayAbility.h"
#include "D1GameplayAbility_Weapon.generated.h"

class AD1WeaponBase;

UCLASS(Blueprintable)
class UD1GameplayAbility_Weapon : public ULyraGameplayAbility
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Weapon(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UFUNCTION(BlueprintCallable)
	void ParseTargetData(const FGameplayAbilityTargetDataHandle& InTargetDataHandle, TArray<int32>& OutAttackHitIndexes, TArray<int32>& OutBlockHitIndexes);

	UFUNCTION(BlueprintCallable)
	void ProcessHitResult(FHitResult HitResult, bool bBlockingHit);
	
	UFUNCTION(BlueprintCallable)
	void DrawDebugHitPoint(const FHitResult& HitResult);

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(GameplayTagFilter = "SetByCaller"))
	int32 GetWeaponStatValue(FGameplayTag StatTag) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetAttackRate() const;
	
protected:
	UPROPERTY(EditDefaultsOnly)
	EWeaponHandType WeaponHandType = EWeaponHandType::Count;

	UPROPERTY(EditDefaultsOnly)
	float DefaultAttackRate = 1.f;

	UPROPERTY(EditDefaultsOnly)
	bool bShowDebug = false;

protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AD1WeaponBase> WeaponActor;
};
