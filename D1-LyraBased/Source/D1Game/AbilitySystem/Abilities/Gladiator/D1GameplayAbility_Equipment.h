#pragma once

#include "D1Define.h"
#include "AbilitySystem/Abilities/LyraGameplayAbility.h"
#include "Character/LyraCharacter.h"
#include "D1GameplayAbility_Equipment.generated.h"

class UD1ItemInstance;
class AD1EquipmentBase;

USTRUCT(BlueprintType)
struct FD1EquipmentInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="D1|Equipment")
	EEquipmentType EquipmentType = EEquipmentType::Count;

public:
	UPROPERTY(EditAnywhere, Category="D1|Equipment", meta=(EditCondition="EquipmentType==EEquipmentType::Armor", EditConditionHides))
	EArmorType RequiredArmorType = EArmorType::Count;

public:
	UPROPERTY(EditAnywhere, Category="D1|Equipment", meta=(EditCondition="EquipmentType==EEquipmentType::Weapon", EditConditionHides))
	EWeaponHandType WeaponHandType = EWeaponHandType::Count;
	
	UPROPERTY(EditAnywhere, Category="D1|Equipment", meta=(EditCondition="EquipmentType==EEquipmentType::Weapon", EditConditionHides))
	EWeaponType RequiredWeaponType = EWeaponType::Count;

public:
	UPROPERTY(EditAnywhere, Category="D1|Equipment", meta=(EditCondition="EquipmentType==EEquipmentType::Utility", EditConditionHides))
	EUtilityType RequiredUtilityType = EUtilityType::Count;

public:
	UPROPERTY()
	TWeakObjectPtr<AD1EquipmentBase> EquipmentActor;
};

UCLASS(Blueprintable)
class UD1GameplayAbility_Equipment : public ULyraGameplayAbility
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Equipment(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;

public:
	AD1EquipmentBase* GetFirstEquipmentActor() const;
	UD1ItemInstance* GetEquipmentItemInstance(const AD1EquipmentBase* InEquipmentActor) const;
	
	int32 GetEquipmentStatValue(FGameplayTag InStatTag, const AD1EquipmentBase* InEquipmentActor) const;
	float GetSnapshottedAttackRate() const { return SnapshottedAttackRate; }
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="D1|Equipment")
	TArray<FD1EquipmentInfo> EquipmentInfos;
	
	UPROPERTY(EditDefaultsOnly, Category="D1|Equipment")
	float DefaultAttackRate = 1.f;

	UPROPERTY(EditDefaultsOnly, Category="D1|Equipment")
	TSubclassOf<ULyraCameraMode> CameraModeClass;

private:
	float SnapshottedAttackRate = 0.f;
};
