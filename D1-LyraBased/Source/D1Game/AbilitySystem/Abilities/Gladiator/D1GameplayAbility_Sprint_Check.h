#pragma once

#include "AbilitySystem/Abilities/LyraGameplayAbility.h"
#include "D1GameplayAbility_Sprint_Check.generated.h"

UCLASS()
class UD1GameplayAbility_Sprint_Check : public ULyraGameplayAbility
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Sprint_Check(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	UFUNCTION(Server, Reliable)
	void Server_RequestSprint(ED1Direction Direction, FVector MovementVector);

private:
	void SendSprintActiveEvent(ED1Direction Direction, FVector MovementVector);
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="D1|Sprint Check")
	FGameplayAttribute SprintCostAttribute;
};
