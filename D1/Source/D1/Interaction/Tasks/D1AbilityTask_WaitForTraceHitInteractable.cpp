#include "D1AbilityTask_WaitForTraceHitInteractable.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AbilityTask_WaitForTraceHitInteractable)

UD1AbilityTask_WaitForTraceHitInteractable::UD1AbilityTask_WaitForTraceHitInteractable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

UD1AbilityTask_WaitForTraceHitInteractable* UD1AbilityTask_WaitForTraceHitInteractable::WaitForTraceHitInteractable(UGameplayAbility* OwningAbility, FCollisionProfileName TraceProfile,
	FGameplayAbilityTargetingLocationInfo StartLocation, float InteractionScanRange, float InteractionScanRate, bool bShowDebug)
{
	UD1AbilityTask_WaitForTraceHitInteractable* TaskObject = NewAbilityTask<UD1AbilityTask_WaitForTraceHitInteractable>(OwningAbility);
	TaskObject->TraceProfile = TraceProfile;
	TaskObject->StartLocation = StartLocation;
	TaskObject->InteractionScanRange = InteractionScanRange;
	TaskObject->InteractionScanRate = InteractionScanRate;
	TaskObject->bShowDebug = bShowDebug;
	return TaskObject;
}

void UD1AbilityTask_WaitForTraceHitInteractable::Activate()
{
	SetWaitingOnAvatar();

	GetWorld()->GetTimerManager().SetTimer(TraceTimerHandle, this, &ThisClass::PerformTrace, InteractionScanRate, true);
}

void UD1AbilityTask_WaitForTraceHitInteractable::OnDestroy(bool bInOwnerFinished)
{
	GetWorld()->GetTimerManager().ClearTimer(TraceTimerHandle);
	
	Super::OnDestroy(bInOwnerFinished);
}

void UD1AbilityTask_WaitForTraceHitInteractable::PerformTrace()
{
	AActor* AvatarActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
	if (AvatarActor == nullptr)
		return;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(AvatarActor);

	FCollisionQueryParams Params(SCENE_QUERY_STAT(UD1AbilityTask_WaitForInteractable)/*Used for Profiling*/, false);
	Params.AddIgnoredActors(ActorsToIgnore);

	FVector TraceStart = StartLocation.GetTargetingTransform().GetLocation();
	FVector TraceEnd;
	
	// AimWitchPlayerController() - Set TraceEnd
	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();

	FVector CameraStart;
	FRotator CameraRotation;
	PC->GetPlayerViewPoint(CameraStart, CameraRotation);

	const FVector CameraDirection = CameraRotation.Vector();
	FVector CameraEnd = CameraStart + (CameraDirection * InteractionScanRange);
	// AimWitchPlayerController();

	// TODO
	// ClipCameraRayToAbilityRange()
	FVector CameraToPawn = TraceStart - CameraStart;
	
	float CameraToDot = FVector::DotProduct(CameraToPawn, CameraDirection/*Unit Vector*/);
	
	float DistanceSquared = CameraToPawn.SizeSquared() - (CameraToDot * CameraToDot);
	float RadiusSquared = (InteractionScanRange * InteractionScanRange);

	// 4 2 => 2
	// 16 4 => root 3.xx
	float DistanceFromCamera = FMath::Sqrt(RadiusSquared - DistanceSquared);
	float DistanceAlongRay = CameraToDot + DistanceFromCamera;

	CameraEnd = CameraStart + (DistanceAlongRay * CameraDirection);
	// ClipCameraRayToAbilityRange()
	
	FHitResult OutHitResult = FHitResult();
	
	TArray<FHitResult> HitResults;
	GetWorld()->LineTraceMultiByProfile(HitResults, TraceStart, TraceEnd, TraceProfile.Name, Params);

	OutHitResult.TraceStart = TraceStart;
	OutHitResult.TraceEnd = TraceEnd;

	if (HitResults.Num() > 0)
	{
		OutHitResult = HitResults[0];
	}

	TArray<TScriptInterface<ID1Interactable>> Interactables;
	// Gathering From HitResult

	UpdateInteractableInfos(Interactables);

#if ENABLE_DRAW_DEBUG
	if (bShowDebug)
	{
		FColor DebugColor = OutHitResult.bBlockingHit ? FColor::Red : FColor::Green;
		if (OutHitResult.bBlockingHit)
		{
			DrawDebugLine(GetWorld(), TraceStart, OutHitResult.Location, DebugColor, false, InteractionScanRate);
			DrawDebugSphere(GetWorld(), OutHitResult.Location, 5.f, 16, DebugColor, false, InteractionScanRate);
		}
		else
		{
			DrawDebugLine(GetWorld(), TraceStart, TraceEnd, DebugColor, false, InteractionScanRate);
		}
	}
#endif // ENABLE_DRAW_DEBUG
}

void UD1AbilityTask_WaitForTraceHitInteractable::UpdateInteractableInfos(const TArray<TScriptInterface<ID1Interactable>>& Interactables)
{
	
}
