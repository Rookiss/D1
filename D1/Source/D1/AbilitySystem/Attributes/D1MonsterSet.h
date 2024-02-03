#pragma once

#include "AbilitySystemComponent.h"
#include "D1AttributeSet.h"
#include "D1MonsterSet.generated.h"

UCLASS(BlueprintType)
class UD1MonsterSet : public UD1AttributeSet
{
	GENERATED_BODY()
	
public:
	UD1MonsterSet();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

private:
	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;
	
protected:
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldValue);

protected:
	UFUNCTION()
	void OnRep_AttackSpeed(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_MoveSpeed(const FGameplayAttributeData& OldValue);
	
private:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Health, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Health;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MaxHealth, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData MaxHealth;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Mana, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Mana;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MaxMana, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData MaxMana;

public:
	ATTRIBUTE_ACCESSORS(ThisClass, Health);
	ATTRIBUTE_ACCESSORS(ThisClass, MaxHealth);
	ATTRIBUTE_ACCESSORS(ThisClass, Mana);
	ATTRIBUTE_ACCESSORS(ThisClass, MaxMana);

private:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_AttackSpeed, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData AttackSpeed;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MoveSpeed, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData MoveSpeed;
	
public:
	ATTRIBUTE_ACCESSORS(ThisClass, AttackSpeed);
	ATTRIBUTE_ACCESSORS(ThisClass, MoveSpeed);

private:
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData BaseDamage;
	
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData BaseDefense;

public:
	ATTRIBUTE_ACCESSORS(ThisClass, BaseDamage);
	ATTRIBUTE_ACCESSORS(ThisClass, BaseDefense);

private:
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData IncomingDamage;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData IncomingHeal;

public:
	ATTRIBUTE_ACCESSORS(ThisClass, IncomingDamage);
	ATTRIBUTE_ACCESSORS(ThisClass, IncomingHeal);

public:
	FOutOfHealthDelegate OutOfHealthDelegate;
	
private:
	bool bOutOfHealth = false;
};
