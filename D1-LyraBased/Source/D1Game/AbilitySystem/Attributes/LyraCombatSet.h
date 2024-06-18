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
	ATTRIBUTE_ACCESSORS(ULyraCombatSet, BaseDamage);
	ATTRIBUTE_ACCESSORS(ULyraCombatSet, BaseHeal);
	
protected:
	UFUNCTION()
	void OnRep_BaseDamage(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_BaseHeal(const FGameplayAttributeData& OldValue);

private:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_BaseDamage, Category = "Lyra|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseDamage;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseHeal, Category = "Lyra|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseHeal;
	
public:
	ATTRIBUTE_ACCESSORS(ThisClass, Strength);
	ATTRIBUTE_ACCESSORS(ThisClass, Vigor);
	ATTRIBUTE_ACCESSORS(ThisClass, Agility);
	ATTRIBUTE_ACCESSORS(ThisClass, Dexterity);
	ATTRIBUTE_ACCESSORS(ThisClass, Will);
	ATTRIBUTE_ACCESSORS(ThisClass, Knowledge);
	ATTRIBUTE_ACCESSORS(ThisClass, Resourcefulness);
	
protected:
	UFUNCTION()
	void OnRep_Strength(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_Vigor(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_Agility(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Dexterity(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_Will(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_Knowledge(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Resourcefulness(const FGameplayAttributeData& OldValue);

private:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Strength, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Strength;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Vigor, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Vigor;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Agility, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Agility;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Dexterity, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Dexterity;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Will, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Will;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Knowledge, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Knowledge;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Resourcefulness, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Resourcefulness;
};
