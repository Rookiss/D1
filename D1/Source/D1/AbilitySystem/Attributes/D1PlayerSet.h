#pragma once

#include "AbilitySystemComponent.h"
#include "D1MonsterSet.h"
#include "D1PlayerSet.generated.h"

UCLASS(BlueprintType)
class UD1PlayerSet : public UD1MonsterSet
{
	GENERATED_BODY()
	
public:
	UD1PlayerSet();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

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

public:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Strength, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Strength;
	ATTRIBUTE_ACCESSORS(UD1PlayerSet, Strength);
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Vigor, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Vigor;
	ATTRIBUTE_ACCESSORS(UD1PlayerSet, Vigor);
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Agility, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Agility;
	ATTRIBUTE_ACCESSORS(UD1PlayerSet, Agility);
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Dexterity, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Dexterity;
	ATTRIBUTE_ACCESSORS(UD1PlayerSet, Dexterity);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Will, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Will;
	ATTRIBUTE_ACCESSORS(UD1PlayerSet, Will);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Knowledge, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Knowledge;
	ATTRIBUTE_ACCESSORS(UD1PlayerSet, Knowledge);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Resourcefulness, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Resourcefulness;
	ATTRIBUTE_ACCESSORS(UD1PlayerSet, Resourcefulness);
};
