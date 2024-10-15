#pragma once

#include "AbilitySystem/Abilities/Weapon/D1GameplayAbility_Weapon.h"
#include "D1GameplayAbility_Skill_Targeting.generated.h"

class AGameplayAbilityWorldReticle;

UCLASS()
class UD1GameplayAbility_Skill_Targeting : public UD1GameplayAbility_Weapon
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Skill_Targeting(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	UFUNCTION(BlueprintCallable)
	void ApplyTarget();
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Spell", meta=(Categories="GameplayCue"))
	FGameplayTag CastGameplayCueTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Spell")
	UAnimMontage* CastStartMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Spell")
	UAnimMontage* CastEndMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Spell")
	UAnimMontage* SpellMontage = nullptr;

protected:
	UPROPERTY(EditDefaultsOnly, meta=(Categories="GameplayCue"))
	FGameplayTag BurstGameplayCueTag;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UGameplayEffect>> GameplayEffectClasses;

private:
	UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	FGameplayAbilityTargetDataHandle TargetDataHandle;
};
