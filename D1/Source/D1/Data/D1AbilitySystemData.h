#pragma once
#include "GameplayTagContainer.h"

#include "D1AbilitySystemData.generated.h"

class UD1AbilitySystemComponent;
struct FActiveGameplayEffectHandle;
struct FGameplayAbilitySpecHandle;
class UGameplayEffect;
class UD1GameplayAbility;

USTRUCT(BlueprintType)
struct FD1AbilitySystemData_Ability
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UD1GameplayAbility> Ability = nullptr;

	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag AbilityTag = FGameplayTag::EmptyTag;
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag = FGameplayTag::EmptyTag;
};

USTRUCT(BlueprintType)
struct FD1AbilitySystemData_Effect
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> Effect = nullptr;

	UPROPERTY(EditDefaultsOnly)
	float EffectLevel = 1.f;
};

USTRUCT(BlueprintType)
struct FD1AbilitySystemData_GrantedHandles
{
	GENERATED_BODY()

public:
	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);
	void AddEffectHandle(const FActiveGameplayEffectHandle& Handle);

	void TakeFromAbilitySystem(UD1AbilitySystemComponent* D1ASC);

protected:
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> EffectHandles;
};

UCLASS(BlueprintType, Const)
class UD1AbilitySystemData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UD1AbilitySystemData(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void GiveToAbilitySystem(UD1AbilitySystemComponent* D1ASC, FD1AbilitySystemData_GrantedHandles* OutGrantedHandles, UObject* SourceObject = nullptr) const;

public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FD1AbilitySystemData_Ability> Abilities;

	UPROPERTY(EditDefaultsOnly)
	TArray<FD1AbilitySystemData_Effect> Effects;
};
