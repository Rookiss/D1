#pragma once

#include "D1GameplayAbility_Weapon.h"
#include "D1GameplayAbility_Weapon_Melee.generated.h"

class AD1WeaponBase;

UCLASS()
class UD1GameplayAbility_Weapon_Melee : public UD1GameplayAbility_Weapon
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Weapon_Melee(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UFUNCTION(BlueprintCallable)
	void ParseTargetData(const FGameplayAbilityTargetDataHandle& InTargetDataHandle, TArray<int32>& OutCharacterHitIndexes, TArray<int32>& OutBlockHitIndexes);

	UFUNCTION(BlueprintCallable)
	void ProcessHitResult(FHitResult HitResult, float Damage, bool bBlockingHit, UAnimMontage* BackwardMontage, AD1WeaponBase* WeaponActor);

	UFUNCTION(BlueprintCallable)
	void ResetHitActors();
	
	UFUNCTION(BlueprintCallable)
	void DrawDebugHitPoint(const FHitResult& HitResult);

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsCharacterBlockingHit(ALyraCharacter* TargetCharacter);
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="D1|Melee")
	float BlockingAngle = 60.f;

	UPROPERTY(EditDefaultsOnly, Category="D1|Melee")
	float BlockHitDamageMultiplier = 0.3f;
	
	UPROPERTY(EditDefaultsOnly, Category="D1|Melee")
	bool bShowDebug = false;

protected:
	UPROPERTY()
	TSet<TWeakObjectPtr<AActor>> CachedHitActors;
};
