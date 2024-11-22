#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "Interaction/D1InteractionQuery.h"
#include "D1AbilityTask_WaitForInteractableTraceHit.generated.h"

struct FD1InteractionInfo;
class ID1Interactable;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractableChanged, const TArray<FD1InteractionInfo>&, InteractableInfos);

UCLASS()
class UD1AbilityTask_WaitForInteractableTraceHit : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	UD1AbilityTask_WaitForInteractableTraceHit(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta=(HidePin="OwningAbility", DefaultToSelf="OwningAbility", BlueprintInternalUseOnly="true"))
	static UD1AbilityTask_WaitForInteractableTraceHit* WaitForInteractableTraceHit(UGameplayAbility* OwningAbility, FD1InteractionQuery InteractionQuery, ECollisionChannel TraceChannel, FGameplayAbilityTargetingLocationInfo StartLocation, float InteractionTraceRange = 100.f, float InteractionTraceRate = 0.1f, bool bShowDebug = false);

protected:
	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

private:
	void PerformTrace();
	
	void AimWithPlayerController(const AActor* InSourceActor, FCollisionQueryParams Params, const FVector& TraceStart, float MaxRange, FVector& OutTraceEnd, bool bIgnorePitch = false) const;
	bool ClipCameraRayToAbilityRange(FVector CameraLocation, FVector CameraDirection, FVector AbilityCenter, float AbilityRange, FVector& OutClippedPosition) const;
	void LineTrace(const FVector& Start, const FVector& End, const FCollisionQueryParams& Params, FHitResult& OutHitResult) const;

	void UpdateInteractionInfos(const FD1InteractionQuery& InteractQuery, const TArray<TScriptInterface<ID1Interactable>>& Interactables);
	void HighlightInteractables(const TArray<FD1InteractionInfo>& InteractionInfos, bool bShouldHighlight);

public:
	UPROPERTY(BlueprintAssignable)
	FOnInteractableChanged InteractableChanged;

private:
	UPROPERTY()
	FD1InteractionQuery InteractionQuery;

	UPROPERTY()
	FGameplayAbilityTargetingLocationInfo StartLocation;

	ECollisionChannel TraceChannel = ECC_Visibility;
	float InteractionTraceRange = 100.f;
	float InteractionTraceRate = 0.1f;
	bool bShowDebug = false;
	
	FTimerHandle TraceTimerHandle;
	TArray<FD1InteractionInfo> CurrentInteractionInfos;
};
