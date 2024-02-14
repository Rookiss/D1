#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "D1AbilityTask_WaitForLineTraceHitInteractable.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractableFocusChanged, const FGameplayAbilityTargetDataHandle&, TargetDataHandle);

UCLASS()
class UD1AbilityTask_WaitForLineTraceHitInteractable : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	UD1AbilityTask_WaitForLineTraceHitInteractable(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UFUNCTION(BlueprintCallable, meta=(HidePin="OwningAbility", DefaultToSelf="OwningAbility", BlueprintInternalUseOnly="true"))
	static UD1AbilityTask_WaitForLineTraceHitInteractable* WaitForLineTraceHitInteractable(UGameplayAbility* OwningAbility, FCollisionProfileName TraceProfile, float InteractionScanRange = 100, float InteractionScanRate = 0.1f, bool bShowDebug = false);

protected:
	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

private:
	void PerformTrace();
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnInteractableFocusChanged InteractableFocusChanged;

private:
	FCollisionProfileName TraceProfile;
	float InteractionScanRange = 100.f;
	float InteractionScanRate = 0.1f;
	bool bShowDebug = false;

private:
	FTimerHandle TraceTimerHandle;
	FGameplayAbilityTargetDataHandle CurrentTargetDataHandle;
};
