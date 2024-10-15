#pragma once

#include "Abilities/GameplayAbilityWorldReticle.h"
#include "AbilitySystem/Abilities/Weapon/D1GameplayAbility_Weapon.h"
#include "D1GameplayAbility_Skill_AOE.generated.h"

class AD1ProjectileBase;

UCLASS()
class UD1GameplayAbility_Skill_AOE : public UD1GameplayAbility_Weapon
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Skill_AOE(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	UFUNCTION(BlueprintCallable)
	void SpawnAOE();

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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Spell Type", DisplayName="AOE Reticle Class")
	TSubclassOf<AGameplayAbilityWorldReticle> AOEReticleClass;

	UPROPERTY(EditDefaultsOnly, Category="Spell Type", DisplayName="AOE Spawner Class")
	TSubclassOf<AActor> AOESpawnerClass;

private:
	UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	FGameplayAbilityTargetDataHandle TargetDataHandle;
};
