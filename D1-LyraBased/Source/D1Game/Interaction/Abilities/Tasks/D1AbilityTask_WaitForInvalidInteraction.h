#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "D1AbilityTask_WaitForInvalidInteraction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInvalidInteraction);

UCLASS()
class UD1AbilityTask_WaitForInvalidInteraction : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	UD1AbilityTask_WaitForInvalidInteraction(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta=(HidePin="OwningAbility", DefaultToSelf="OwningAbility", BlueprintInternalUseOnly="true"))
	static UD1AbilityTask_WaitForInvalidInteraction* WaitForInvalidInteraction(UGameplayAbility* OwningAbility, float AcceptanceAngle, float AcceptanceDistance);

protected:
	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

private:
	void PerformCheck();
	float CalculateAngle2D() const;

public:
	UPROPERTY(BlueprintAssignable)
	FOnInvalidInteraction OnInvalidInteraction;

private:
	float AcceptanceAngle = 0.f;
	float AcceptanceDistance = 0.f;
	
private:
	FTimerHandle CheckTimerHandle;
	FVector CachedCharacterForward2D;
	FVector CachedCharacterLocation;
};
