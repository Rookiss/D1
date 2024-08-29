#pragma once

#include "D1GameplayAbility_Weapon.h"
#include "D1GameplayAbility_Weapon_Projectile.generated.h"

class AD1ProjectileBase;

UCLASS()
class UD1GameplayAbility_Weapon_Projectile : public UD1GameplayAbility_Weapon
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Weapon_Projectile(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UFUNCTION(BlueprintCallable)
	void SpawnProjectile();

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
	UPROPERTY(EditDefaultsOnly, Category="Spell Type")
	TArray<TSubclassOf<AD1ProjectileBase>> ProjectileClasses;

	UPROPERTY(EditDefaultsOnly, Category="Spell Type")
	FName ProjectileSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Spell Type")
	TArray<float> PhaseTimes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Spell Type")
	TArray<FLinearColor> PhaseColors;
	
private:
	UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	int32 CurrentIndex = 0;
};
