#pragma once

#include "AbilitySystem/Abilities/LyraGameplayAbility.h"
#include "D1GameplayAbility_Crouch.generated.h"

UCLASS()
class UD1GameplayAbility_Crouch : public ULyraGameplayAbility
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Crouch(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

public:
	bool CanCrouch() const;
	
private:
	UFUNCTION()
	void OnInputReleased(float TimeHeld);

	UFUNCTION()
	void OnTick(float DeltaTime);
};
