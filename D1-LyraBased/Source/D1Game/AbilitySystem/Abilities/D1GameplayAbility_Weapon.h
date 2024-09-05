#pragma once

#include "D1Define.h"
#include "LyraGameplayAbility.h"
#include "Character/LyraCharacter.h"
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
	void ParseTargetData(const FGameplayAbilityTargetDataHandle& InTargetDataHandle, TArray<int32>& OutCharacterHitIndexes, TArray<int32>& OutBlockHitIndexes);

	UFUNCTION(BlueprintCallable)
	void ProcessHitResult(FHitResult HitResult, float Damage, bool bBlockingHit, UAnimMontage* BackwardMontage);

	UFUNCTION(BlueprintCallable)
	void ResetHitActors();
	
	UFUNCTION(BlueprintCallable)
	void DrawDebugHitPoint(const FHitResult& HitResult);

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsBlockingHit(ALyraCharacter* TargetCharacter);
	
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
	float BlockingAngle = 60.f;
	
	UPROPERTY(EditDefaultsOnly)
	bool bShowDebug = false;

protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AD1WeaponBase> WeaponActor;

	UPROPERTY()
	TSet<TWeakObjectPtr<AActor>> HitActors;
};
