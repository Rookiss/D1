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
	void OnRep_Will(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_Vigor(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_Agility(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_Knowledge(const FGameplayAttributeData& OldValue);

public:
	// 물리 피해 보너스
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Strength, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Strength;
	ATTRIBUTE_ACCESSORS(UD1PlayerSet, Strength);

	// 마법 피해 보너스
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Will, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Will;
	ATTRIBUTE_ACCESSORS(UD1PlayerSet, Will);
	
	// 최대 체력 보너스
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Vigor, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Vigor;
	ATTRIBUTE_ACCESSORS(UD1PlayerSet, Vigor);

	// 물리 공격 속도와 이동 속도 보너스
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Agility, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Agility;
	ATTRIBUTE_ACCESSORS(UD1PlayerSet, Agility);
	
	// 마법 공격 속도와 최대 마나 보너스
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Knowledge, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Knowledge;
	ATTRIBUTE_ACCESSORS(UD1PlayerSet, Knowledge);
};
