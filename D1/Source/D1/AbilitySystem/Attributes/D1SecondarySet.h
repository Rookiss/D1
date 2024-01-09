#pragma once

#include "AbilitySystemComponent.h"
#include "D1AttributeSet.h"
#include "D1SecondarySet.generated.h"

UCLASS(BlueprintType)
class UD1SecondarySet : public UD1AttributeSet
{
	GENERATED_BODY()
	
public:
	UD1SecondarySet();

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
	ATTRIBUTE_ACCESSORS(UD1SecondarySet, Strength);
	FAttributeChangedDelegate StrengthChangedDelegate;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Vigor, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Vigor;
	ATTRIBUTE_ACCESSORS(UD1SecondarySet, Vigor);
	FAttributeChangedDelegate VigorChangedDelegate;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Agility, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Agility;
	ATTRIBUTE_ACCESSORS(UD1SecondarySet, Agility);
	FAttributeChangedDelegate AgilityChangedDelegate;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Dexterity, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Dexterity;
	ATTRIBUTE_ACCESSORS(UD1SecondarySet, Dexterity);
	FAttributeChangedDelegate DexterityChangedDelegate;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Will, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Will;
	ATTRIBUTE_ACCESSORS(UD1SecondarySet, Will);
	FAttributeChangedDelegate WillChangedDelegate;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Knowledge, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Knowledge;
	ATTRIBUTE_ACCESSORS(UD1SecondarySet, Knowledge);
	FAttributeChangedDelegate KnowledgeChangedDelegate;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Resourcefulness, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Resourcefulness;
	ATTRIBUTE_ACCESSORS(UD1SecondarySet, Resourcefulness);
	FAttributeChangedDelegate ResourcefulnessChangedDelegate;
};
