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

public:
	ATTRIBUTE_ACCESSORS(ThisClass, BaseDamage);
	ATTRIBUTE_ACCESSORS(ThisClass, BaseHeal);

protected:
	UFUNCTION()
	void OnRep_BaseDamage(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_BaseHeal(const FGameplayAttributeData& OldValue);

private:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_BaseDamage, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData BaseDamage;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_BaseHeal, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData BaseHeal;
	
public:
	ATTRIBUTE_ACCESSORS(ThisClass, BaseMoveSpeed);

protected:
	UFUNCTION()
	void OnRep_BaseMoveSpeed(const FGameplayAttributeData& OldValue);

private:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_BaseMoveSpeed, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData BaseMoveSpeed;
	
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
