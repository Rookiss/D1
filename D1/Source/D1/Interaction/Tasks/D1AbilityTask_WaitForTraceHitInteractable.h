#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "Interaction/D1InteractionInfo.h"
#include "D1AbilityTask_WaitForTraceHitInteractable.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractableChanged, const TArray<FD1InteractionInfo>&, InteractionInfos);

UCLASS(Abstract)
class UD1AbilityTask_WaitForTraceHitInteractable : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	UD1AbilityTask_WaitForTraceHitInteractable(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UFUNCTION(BlueprintCallable, meta=(HidePin="OwingAbility", DefaultToSelf="OwningAbility", BlueprintInternalUseOnly="true"))
	static UD1AbilityTask_WaitForTraceHitInteractable* WaitForTraceHitInteractable(UGameplayAbility* OwningAbility, FCollisionProfileName TraceProfile, FGameplayAbilityTargetingLocationInfo StartLocation, float InteractionScanRange = 100, float InteractionScanRate = 0.1f, bool bShowDebug = false);
	
protected:
	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

private:
	void PerformTrace();
	void UpdateInteractableInfos(const TArray<TScriptInterface<ID1Interactable>>& Interactables);
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnInteractableChanged OnInteractableChanged;

private:
	UPROPERTY()
	FGameplayAbilityTargetingLocationInfo StartLocation;

	FTimerHandle TraceTimerHandle;
	FCollisionProfileName TraceProfile;
	float InteractionScanRange = 100.f;
	float InteractionScanRate = 0.1f;
	bool bShowDebug = false;
};
