#pragma once

#include "D1Define.h"
#include "AbilitySystem/Abilities/LyraGameplayAbility.h"
#include "Character/LyraCharacter.h"
#include "D1GameplayAbility_Weapon.generated.h"

class AD1WeaponBase;

USTRUCT(BlueprintType)
struct FD1WeaponInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="D1|Weapon")
	EWeaponHandType WeaponHandType = EWeaponHandType::Count;
	
	UPROPERTY(EditDefaultsOnly, Category="D1|Weapon")
	bool bShouldCheckWeaponType = true;
	
	UPROPERTY(EditDefaultsOnly, Category="D1|Weapon", meta=(EditCondition="bShouldCheckWeaponType", EditConditionHides))
	EWeaponType RequiredWeaponType = EWeaponType::Count;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AD1WeaponBase> WeaponActor;
};

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
	UFUNCTION(BlueprintCallable)
	AD1WeaponBase* GetFirstWeaponActor() const;
	
	int32 GetWeaponStatValue(FGameplayTag InStatTag, const AD1WeaponBase* InWeaponActor) const;

	UFUNCTION(BlueprintCallable)
	float GetSnapshottedAttackRate() const { return SnapshottedAttackRate; }
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="D1|Weapon")
	TArray<FD1WeaponInfo> WeaponInfos;
	
	UPROPERTY(EditDefaultsOnly, Category="D1|Weapon")
	float DefaultAttackRate = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="D1|Weapon")
	TSubclassOf<ULyraCameraMode> CameraModeClass;

private:
	float SnapshottedAttackRate = 0.f;
};
