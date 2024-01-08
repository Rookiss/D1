#pragma once

#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpecHandle.h"
#include "D1GlobalAbilitySystem.generated.h"

class UGameplayEffect;
class UGameplayAbility;
class UD1AbilitySystemComponent;

USTRUCT()
struct FGlobalAppliedAbilityList
{
	GENERATED_BODY()

public:
	void AddToASC(TSubclassOf<UGameplayAbility> Ability, UD1AbilitySystemComponent* ASC);
	void RemoveFromASC(UD1AbilitySystemComponent* ASC);
	void RemoveFromAll();

public:
	UPROPERTY()
	TMap<TObjectPtr<UD1AbilitySystemComponent>, FGameplayAbilitySpecHandle> Handles;
};

USTRUCT()
struct FGlobalAppliedEffectList
{
	GENERATED_BODY()

public:
	void AddToASC(TSubclassOf<UGameplayEffect> Effect, UD1AbilitySystemComponent* ASC);
	void RemoveFromASC(UD1AbilitySystemComponent* ASC);
	void RemoveFromAll();

public:
	UPROPERTY()
	TMap<TObjectPtr<UD1AbilitySystemComponent>, FActiveGameplayEffectHandle> Handles;
};

UCLASS()
class UD1GlobalAbilitySystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	UD1GlobalAbilitySystem();

public:
	void RegisterASC(UD1AbilitySystemComponent* ASC);
	void UnregisterASC(UD1AbilitySystemComponent* ASC);

public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void ApplyAbilityToAll(TSubclassOf<UGameplayAbility> Ability);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void ApplyEffectToAll(TSubclassOf<UGameplayEffect> Effect);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void RemoveAbilityFromAll(TSubclassOf<UGameplayAbility> Ability);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void RemoveEffectFromAll(TSubclassOf<UGameplayEffect> Effect);
	
public:
	UPROPERTY()
	TMap<TSubclassOf<UGameplayAbility>, FGlobalAppliedAbilityList> AppliedAbilities;

	UPROPERTY()
	TMap<TSubclassOf<UGameplayEffect>, FGlobalAppliedEffectList> AppliedEffects;

	UPROPERTY()
	TArray<TObjectPtr<UD1AbilitySystemComponent>> RegisteredASCs;
};
