#pragma once

#include "AbilitySystem/Abilities/LyraGameplayAbility.h"
#include "D1GameplayAbility_Jump.generated.h"

struct FGameplayAbilityActorInfo;
struct FGameplayTagContainer;

UCLASS(Abstract)
class UD1GameplayAbility_Jump : public ULyraGameplayAbility
{
	GENERATED_BODY()

public:
	UD1GameplayAbility_Jump(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UFUNCTION(Server, Reliable)
	void Server_RequestJump();
	
protected:
	UFUNCTION()
	void StartJump();
};
