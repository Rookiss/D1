#pragma once

#include "AbilitySystemComponent.h"
#include "LyraAttributeSet.h"
#include "LyraCombatSet.generated.h"

UCLASS(BlueprintType)
class ULyraCombatSet : public ULyraAttributeSet
{
	GENERATED_BODY()

public:
	ULyraCombatSet();

protected:
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;
	
public:
	ATTRIBUTE_ACCESSORS(ThisClass, BaseDamage);
	ATTRIBUTE_ACCESSORS(ThisClass, BaseHealHealth);
	ATTRIBUTE_ACCESSORS(ThisClass, BaseHealMana);
	ATTRIBUTE_ACCESSORS(ThisClass, BaseHealStamina);
	ATTRIBUTE_ACCESSORS(ThisClass, MoveSpeed);
	ATTRIBUTE_ACCESSORS(ThisClass, MoveSpeedPercent);

protected:
	UFUNCTION()
	void OnRep_BaseDamage(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_BaseHealHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_BaseHealMana(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_BaseHealStamina(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MoveSpeed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MoveSpeedPercent(const FGameplayAttributeData& OldValue);
	
private:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_BaseDamage, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData BaseDamage;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_BaseHealHealth, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData BaseHealHealth;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_BaseHealMana, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData BaseHealMana;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_BaseHealStamina, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData BaseHealStamina;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MoveSpeed, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData MoveSpeed;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MoveSpeedPercent, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData MoveSpeedPercent;

public:
	ATTRIBUTE_ACCESSORS(ThisClass, Mana);
	ATTRIBUTE_ACCESSORS(ThisClass, MaxMana);
	ATTRIBUTE_ACCESSORS(ThisClass, Stamina);
	ATTRIBUTE_ACCESSORS(ThisClass, MaxStamina);

protected:
	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Stamina(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxStamina(const FGameplayAttributeData& OldValue);
	
private:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Mana, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Mana;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MaxMana, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData MaxMana;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Stamina, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Stamina;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MaxStamina, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData MaxStamina;

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
	
	// 최대 체력(+)
	UFUNCTION()
	void OnRep_Vigor(const FGameplayAttributeData& OldValue);

	// 이동 속도(+)
	UFUNCTION()
	void OnRep_Agility(const FGameplayAttributeData& OldValue);
	
	// 상호작용 속도(+)
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
};
