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
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(GameplayTagFilter = "SetByCaller"))
	int32 GetWeaponStatValue(FGameplayTag StatTag) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetAttackRate() const;
	
protected:
	UPROPERTY(EditDefaultsOnly)
	EWeaponHandType WeaponHandType = EWeaponHandType::Count;

	UPROPERTY(EditDefaultsOnly)
	bool bShouldCheckWeaponType = true;
	
	UPROPERTY(EditDefaultsOnly, meta=(EditCondition="bShouldCheckWeaponType", EditConditionHides))
	EWeaponType RequiredWeaponType = EWeaponType::Count;
	
	UPROPERTY(EditDefaultsOnly)
	float DefaultAttackRate = 1.f;
	
protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AD1WeaponBase> WeaponActor;
};
