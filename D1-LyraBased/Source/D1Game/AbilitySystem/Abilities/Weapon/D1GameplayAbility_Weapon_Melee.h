#pragma once

#include "AbilitySystem/Abilities/Gladiator/D1GameplayAbility_Equipment.h"
#include "D1GameplayAbility_Weapon_Melee.generated.h"

class AD1EquipmentBase;

UCLASS()
class UD1GameplayAbility_Weapon_Melee : public UD1GameplayAbility_Equipment
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Weapon_Melee(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UFUNCTION()
	void ParseTargetData(const FGameplayAbilityTargetDataHandle& InTargetDataHandle, TArray<int32>& OutCharacterHitIndexes, TArray<int32>& OutBlockHitIndexes);

	UFUNCTION()
	void ProcessHitResult(FHitResult HitResult, float Damage, bool bBlockingHit, UAnimMontage* BackwardMontage, AD1EquipmentBase* WeaponActor);

	UFUNCTION()
	void ResetHitActors();
	
	UFUNCTION()
	void DrawDebugHitPoint(const FHitResult& HitResult);

public:
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
