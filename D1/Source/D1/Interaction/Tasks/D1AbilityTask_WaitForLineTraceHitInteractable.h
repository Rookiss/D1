#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "Interaction/D1Interactable.h"
#include "D1AbilityTask_WaitForLineTraceHitInteractable.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractableChanged, const FD1InteractionInfo&, InteractionInfo);

UCLASS()
class UD1AbilityTask_WaitForLineTraceHitInteractable : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	UD1AbilityTask_WaitForLineTraceHitInteractable(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UFUNCTION(BlueprintCallable, meta=(HidePin="OwningAbility", DefaultToSelf="OwningAbility", BlueprintInternalUseOnly="true"))
	static UD1AbilityTask_WaitForLineTraceHitInteractable* WaitForLineTraceHitInteractable(UGameplayAbility* OwningAbility, FCollisionProfileName TraceProfile, float InteractionScanRange = 100, float InteractionScanRate = 0.1f);

protected:
	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

private:
	void PerformTrace();
	void HandleInteractableInfo(const TScriptInterface<ID1Interactable>& Interactable);
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnInteractableChanged OnInteractableChanged;

private:
	FCollisionProfileName TraceProfile;
	float InteractionScanRange = 100.f;
	float InteractionScanRate = 0.1f;
	
	FTimerHandle TraceTimerHandle;
	FD1InteractionInfo LatestInfo;
};
