#pragma once

#include "D1GameplayAbility_Weapon.h"
#include "Abilities/GameplayAbilityWorldReticle.h"
#include "D1GameplayAbility_Weapon_Spell.generated.h"

class AD1ProjectileBase;

UCLASS()
class UD1GameplayAbility_Weapon_Spell : public UD1GameplayAbility_Weapon
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Weapon_Spell(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UFUNCTION(BlueprintCallable)
	void SpawnProjectile();

	UFUNCTION(BlueprintCallable)
	void SpawnAOE();
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Spell Setting")
	float CastTime = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Spell Setting")
	TSubclassOf<UGameplayEffect> SpellActiveGameplayEffect;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Spell Montage")
	UAnimMontage* CastStartMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Spell Montage")
	UAnimMontage* CastEndMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Spell Montage")
	UAnimMontage* SpellMontage = nullptr;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Spell Type")
	ESpellType SpellType = ESpellType::None;
	
	UPROPERTY(EditDefaultsOnly, Category="Spell Type", meta=(EditCondition="SpellType==ESpellType::Projectile", EditConditionHides))
	TSubclassOf<AD1ProjectileBase> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category="Spell Type", meta=(EditCondition="SpellType==ESpellType::Projectile", EditConditionHides))
	FName ProjectileSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Spell Type", DisplayName="AOE Reticle Class", meta=(EditCondition="SpellType==ESpellType::AOE", EditConditionHides))
	TSubclassOf<AGameplayAbilityWorldReticle> AOEReticleClass;

	UPROPERTY(EditDefaultsOnly, Category="Spell Type", DisplayName="AOE Spawner Class", meta=(EditCondition="SpellType==ESpellType::AOE", EditConditionHides))
	TSubclassOf<AActor> AOESpawnerClass;

private:
	UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	bool bCastTimePassed = false;
	
	UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	FGameplayAbilityTargetDataHandle TargetDataHandle;

	UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	FActiveGameplayEffectHandle ActiveGameplayEffectHandle;
};
