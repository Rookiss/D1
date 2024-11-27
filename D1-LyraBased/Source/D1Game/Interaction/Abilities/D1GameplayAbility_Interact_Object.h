#pragma once

#include "D1GameplayAbility_Interact_Info.h"
#include "D1GameplayAbility_Interact_Object.generated.h"

UCLASS()
class UD1GameplayAbility_Interact_Object : public UD1GameplayAbility_Interact_Info
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Interact_Object(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
protected:
	UFUNCTION()
	void OnInvalidInteraction();

protected:
	UPROPERTY(EditDefaultsOnly, Category="D1|Interaction")
	float AcceptanceAngle = 90.f;

	UPROPERTY(EditDefaultsOnly, Category="D1|Interaction")
	float AcceptanceDistance = 100.f;
	
protected:
	bool bInitialized = false;
};
