#pragma once

#include "GameplayAbilitySpecHandle.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayTagContainer.h"
#include "D1AbilitySystemData.generated.h"

class UAttributeSet;
class UD1AbilitySystemComponent;
class UGameplayEffect;
class UD1GameplayAbility;

USTRUCT()
struct FD1AbilitySystemData_Ability
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UD1GameplayAbility> AbilityClass = nullptr;

	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag = FGameplayTag::EmptyTag;
};

USTRUCT()
struct FD1AbilitySystemData_Effect
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> EffectClass = nullptr;

	UPROPERTY(EditDefaultsOnly)
	float EffectLevel = 1.f;
};

USTRUCT()
struct FD1AbilitySystemData_AttributeSet
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAttributeSet> AttributeSetClass;
};

USTRUCT()
struct FD1AbilitySystemData_GrantedHandles
{
	GENERATED_BODY()
	
public:
	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);
	void AddEffectHandle(const FActiveGameplayEffectHandle& Handle);
	void AddAttributeSet(UAttributeSet* AttributeSet);
	
	void TakeFromAbilitySystem(UD1AbilitySystemComponent* ASC);

protected:
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> GrantedAbilitySpecHandles;

	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> GrantedEffectHandles;

	UPROPERTY()
	TArray<TObjectPtr<UAttributeSet>> GrantedAttributeSets;
};

UCLASS(Const)
class UD1AbilitySystemData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UD1AbilitySystemData(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void GiveToAbilitySystem(UD1AbilitySystemComponent* ASC, FD1AbilitySystemData_GrantedHandles* OutGrantedHandles, UObject* SourceObject = nullptr) const;
	
public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FD1AbilitySystemData_Ability> AbilitiesToGrant;

	UPROPERTY(EditDefaultsOnly)
	TArray<FD1AbilitySystemData_Ability> AbilitiesToPending;

	UPROPERTY(EditDefaultsOnly)
	TArray<FD1AbilitySystemData_Effect> EffectsToGrant;

	UPROPERTY(EditDefaultsOnly)
	TArray<FD1AbilitySystemData_AttributeSet> AttributeSetsToGrant;
};
