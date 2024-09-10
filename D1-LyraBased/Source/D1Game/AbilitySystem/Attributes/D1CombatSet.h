#pragma once

#include "AbilitySystemComponent.h"
#include "D1AttributeSet.h"
#include "D1CombatSet.generated.h"

UCLASS(BlueprintType)
class UD1CombatSet : public UD1AttributeSet
{
	GENERATED_BODY()

public:
	UD1CombatSet();

protected:
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

private:
	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;
	
public:
	ATTRIBUTE_ACCESSORS(ThisClass, BaseDamage);
	ATTRIBUTE_ACCESSORS(ThisClass, BaseHealth);
	ATTRIBUTE_ACCESSORS(ThisClass, BaseMana);
	ATTRIBUTE_ACCESSORS(ThisClass, BaseStamina);

protected:
	UFUNCTION()
	void OnRep_BaseDamage(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_BaseHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_BaseMana(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_BaseStamina(const FGameplayAttributeData& OldValue);

private:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_BaseDamage, meta=(HideFromModifiers, AllowPrivateAccess="true"))
	FGameplayAttributeData BaseDamage;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_BaseHealth, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData BaseHealth;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_BaseMana, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData BaseMana;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_BaseStamina, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData BaseStamina;

public:
	ATTRIBUTE_ACCESSORS(ThisClass, MoveSpeed);
	ATTRIBUTE_ACCESSORS(ThisClass, MoveSpeedPercent);
	ATTRIBUTE_ACCESSORS(ThisClass, AttackSpeedPercent);
	ATTRIBUTE_ACCESSORS(ThisClass, DrainLifePercent);
	ATTRIBUTE_ACCESSORS(ThisClass, DamageReductionPercent);

protected:
	UFUNCTION()
	void OnRep_MoveSpeed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MoveSpeedPercent(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_AttackSpeedPercent(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_DrainLifePercent(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_DamageReductionPercent(const FGameplayAttributeData& OldValue);

private:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MoveSpeed, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData MoveSpeed;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MoveSpeedPercent, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData MoveSpeedPercent;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_AttackSpeedPercent, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData AttackSpeedPercent;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_DrainLifePercent, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData DrainLifePercent;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_DrainLifePercent, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData DamageReductionPercent;
	
public:
	ATTRIBUTE_ACCESSORS(ThisClass, Strength);
	ATTRIBUTE_ACCESSORS(ThisClass, Defense);
	ATTRIBUTE_ACCESSORS(ThisClass, Vigor);
	ATTRIBUTE_ACCESSORS(ThisClass, Agility);
	ATTRIBUTE_ACCESSORS(ThisClass, Resourcefulness);
	
protected:
	// 공격력
	UFUNCTION()
	void OnRep_Strength(const FGameplayAttributeData& OldValue);

	// 방어력
	UFUNCTION()
	void OnRep_Defense(const FGameplayAttributeData& OldValue);
	
	// 최대 체력
	UFUNCTION()
	void OnRep_Vigor(const FGameplayAttributeData& OldValue);

	// 이동 속도
	UFUNCTION()
	void OnRep_Agility(const FGameplayAttributeData& OldValue);
	
	// 상호작용 속도
	UFUNCTION()
	void OnRep_Resourcefulness(const FGameplayAttributeData& OldValue);

private:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Strength, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Strength;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Defense, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Defense;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Vigor, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Vigor;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Agility, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Agility;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Resourcefulness, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Resourcefulness;

public:
	ATTRIBUTE_ACCESSORS(ThisClass, ActiveEffectDuration);

private:
	UPROPERTY(BlueprintReadOnly, meta=(HideFromModifiers, AllowPrivateAccess="true"))
	FGameplayAttributeData ActiveEffectDuration;
};
