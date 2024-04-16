#pragma once

#include "AbilitySystemComponent.h"
#include "D1AbilitySystemComponent.generated.h"

class UD1GameplayAbility;

DECLARE_MULTICAST_DELEGATE_TwoParams(FAbilityChangedDelegate, bool, const FGameplayTag&);

UCLASS(BlueprintType)
class UD1AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	UD1AbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;

	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	virtual void OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	
	void TryActivateAbilitiesOnSpawn();

	typedef TFunctionRef<bool(const UD1GameplayAbility* Ability, FGameplayAbilitySpecHandle Handle)> TShouldCancelAbilityFunc;
	void CancelAbilitiesByFunc(TShouldCancelAbilityFunc ShouldCancelFunc, bool bReplicateCancelAbility);

	void CancelInputActivatedAbilities(bool bReplicateCancelAbility);

	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;
	
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	
	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
	void ClearAbilityInput();

	UFUNCTION(BlueprintCallable)
	void AddDynamicTagToSelf(const FGameplayTag& Tag);

	UFUNCTION(BlueprintCallable)
	void RemoveDynamicTagToSelf(const FGameplayTag& Tag);
		
	UFUNCTION(BlueprintCallable, meta=(AutoCreateRefTerm="GameplayCueParameters", GameplayTagFilter="GameplayCue"))
	void ExecuteGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters);

	UFUNCTION(BlueprintCallable, meta=(AutoCreateRefTerm="GameplayCueParameters", GameplayTagFilter="GameplayCue"))
	void AddGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters);

	UFUNCTION(BlueprintCallable, meta=(AutoCreateRefTerm="GameplayCueParameters", GameplayTagFilter="GameplayCue"))
	void RemoveGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters);
	
	void BlockAnimMontageForSeconds(UAnimMontage* BackwardMontage);
	
	UFUNCTION(BlueprintCallable, NetMulticast, Unreliable)
	void Multicast_BlockAnimMontageForSeconds(UAnimMontage* BackwardMontage);
	
	void SlowAnimMontageForSeconds(UAnimMontage* AttackMontage);
	
	UFUNCTION(BlueprintCallable, NetMulticast, Unreliable)
	void Multicast_SlowAnimMontageForSeconds(UAnimMontage* AttackMontage);
	
public:
	FTimerHandle SlowAnimMontageTimerHandle;
	FTimerHandle BlockAnimMontageTimerHandle;
	FAbilityChangedDelegate AbilityChangedDelegate;
	
private:
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;
};
